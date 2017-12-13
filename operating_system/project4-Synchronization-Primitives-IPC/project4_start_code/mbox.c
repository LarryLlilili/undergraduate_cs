#include "common.h"
#include "mbox.h"
#include "sync.h"
#include "scheduler.h"
#include "interrupt.h"

typedef struct
{
	/* TODO */
  char msg[MAX_MESSAGE_LENGTH];
} Message;

typedef struct
{
	/* TODO */
  char name[MBOX_NAME_LENGTH];
  Message box[MAX_MBOX_LENGTH];
  uint32_t use_count;
  uint32_t msg_count;
  lock_t l;//protect the mail_box
  node_t send_wait_queue;
  node_t recv_wait_queue;
  int read_place;
  int write_place; 
} MessageBox;

MessageBox mail_box[MAX_MBOXEN];

static MessageBox MessageBoxen[MAX_MBOXEN];
lock_t BoxLock;

/* Perform any system-startup
 * initialization for the message
 * boxes.
 */

void unblock_mail(pcb_t* mypcb){
	int i;
	bool_t j;
	for(i=0;i<MAX_MBOXEN;++i){
		if(mypcb->mailbox[i]==TRUE){
			j=current_running->mailbox[i];
			do_mbox_close(i);
			current_running->mailbox[i]=j;
		}
	}
}


void init_mbox_one(int i){
	mail_box[i].use_count=0;
	mail_box[i].msg_count=0;
	mail_box[i].read_place=0;
	mail_box[i].write_place=0;
	queue_init(&mail_box[i].send_wait_queue);
	queue_init(&mail_box[i].recv_wait_queue);
	lock_init(&mail_box[i].l);
	mail_box[i].name[0]='\0';
}
void init_mbox(void)
{
	/* TODO */
	int i;
	for(i=0;i<MAX_MBOXEN;++i){
		init_mbox_one(i);
	}
}

/* Opens the mailbox named 'name', or
 * creates a new message box if it
 * doesn't already exist.
 * A message box is a bounded buffer
 * which holds up to MAX_MBOX_LENGTH items.
 * If it fails because the message
 * box table is full, it will return -1.
 * Otherwise, it returns a message box
 * id.
 */
mbox_t do_mbox_open(const char *name)//typedef int mbox_t
{
  (void)name;
	/* TODO */
	int i,j;
	for (i=0;i<MAX_MBOXEN;++i){
		if(same_string(name,mail_box[i].name)){
			current_running->mailbox[i]=TRUE;
			mail_box[i].use_count++;
			return i;
		}
	}
	for(i=0;i<MAX_MBOXEN;++i){
		j=strlen(name);
		if(strlen(mail_box[i].name)==0){
			bcopy(name,mail_box[i].name,j);
			current_running->mailbox[i]=TRUE;
			return i;
		}
    }
} 


/* Closes a message box
 */
void do_mbox_close(mbox_t mbox)
{
	(void)mbox;
	/* TODO */
	mail_box[mbox].use_count--;
	current_running->mailbox[mbox]=FALSE;
	if(mail_box[mbox].use_count==0)
		init_mbox_one(mbox);
}

/* Determine if the given
 * message box is full.
 * Equivalently, determine
 * if sending to this mbox
 * would cause a process
 * to block.
 */
int do_mbox_is_full(mbox_t mbox)
{
	(void)mbox;
	/* TODO */

	if(mail_box[mbox].msg_count==MAX_MBOX_LENGTH){
		enter_critical();
		block(&mail_box[mbox].send_wait_queue);
		leave_critical();
		return 1;
	}
	return 0;
}

int do_mbox_is_empty(mbox_t mbox)
{
	(void)mbox;
	/* TODO */

	if(mail_box[mbox].msg_count==0){
		enter_critical();
		block(&mail_box[mbox].recv_wait_queue);
		leave_critical();
		return 1;
	}
	return 0;
}
/* Enqueues a message onto
 * a message box.  If the
 * message box is full, the
 * process will block until
 * it can add the item.
 * You may assume that the
 * message box ID has been
 * properly opened before this
 * call.
 * The message is 'nbytes' bytes
 * starting at offset 'msg'
 */
void do_mbox_send(mbox_t mbox, void *msg, int nbytes)
{
	(void)mbox;
	(void)msg;
	(void)nbytes;
	 /* TODO */
	int i;
	pcb_t *p;
	do_mbox_is_full(mbox);
	lock_acquire(&mail_box[mbox].l);
	for(i=0;i<nbytes && i<MAX_MESSAGE_LENGTH ;++i)
		mail_box[mbox].box[mail_box[mbox].write_place].msg[i]=((char*)msg)[i];
	mail_box[mbox].write_place++;
	mail_box[mbox].write_place = mail_box[mbox].write_place % MAX_MBOX_LENGTH;
	mail_box[mbox].msg_count++;
	lock_release(&mail_box[mbox].l);
	
	enter_critical();
	ASSERT(disable_count);
	if(!is_empty(&mail_box[mbox].recv_wait_queue)){
		p=(pcb_t*)dequeue(&mail_box[mbox].recv_wait_queue);
		unblock(p);
	}
	leave_critical();

}

/* Receives a message from the
 * specified message box.  If
 * empty, the process will block
 * until it can remove an item.
 * You may assume that the
 * message box has been properly
 * opened before this call.
 * The message is copied into
 * 'msg'.  No more than
 * 'nbytes' bytes will by copied
 * into this buffer; longer
 * messages will be truncated.
 */
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes)
{
	(void)mbox;
	(void)msg;
	(void)nbytes;
  /* TODO */
	int i;
	pcb_t *p;
	do_mbox_is_empty(mbox);

	lock_acquire(&mail_box[mbox].l);
	for(i=0;i<nbytes && i<MAX_MESSAGE_LENGTH ;++i)
		((char*)msg)[i]=mail_box[mbox].box[mail_box[mbox].read_place].msg[i];
	mail_box[mbox].read_place++;
	mail_box[mbox].read_place = mail_box[mbox].read_place % MAX_MBOX_LENGTH;
	mail_box[mbox].msg_count--;
	lock_release(&mail_box[mbox].l);
	
	enter_critical();
	ASSERT(disable_count);
	if(!is_empty(&mail_box[mbox].send_wait_queue)){
		p=(pcb_t*)dequeue(&mail_box[mbox].send_wait_queue);
		unblock(p);
	}
	leave_critical();

}


