#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_bootblock(FILE **image_file, FILE *boot_file, Elf32_Ehdr *boot_ehar, Elf32_Phdr *boot_phdr)
{
	Elf32_Word size = boot_phdr->p_filesz;
	uint8_t buf[size];
	fseek(boot_file, boot_phdr->p_offset, SEEK_SET);   
	fread(buf, size, 1, boot_file);
	fwrite(buf, 1, size, *image_file);
}

void write_kernel(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr) 
{
	Elf32_Word size = kernel_phdr->p_filesz;
	uint8_t buf[size];
	fseek(kernel_file, kernel_phdr->p_offset, SEEK_SET);    
	fread(buf, size, 1, kernel_file);
	fseek(*image_file, 512, SEEK_SET);
	fwrite(buf, 1, size, *image_file);
}

void write_task(FILE **image_file, FILE *task_file, Elf32_Ehdr *task_ehdr, Elf32_Phdr *task_phdr, 
	uint32_t addr)
{
	Elf32_Word size = task_phdr->p_filesz;
	uint8_t buf[size];
	fseek(task_file, task_phdr->p_offset, SEEK_SET);
	fread(buf, size, 1, task_file);
	fseek(*image_file, addr, SEEK_SET);  
	fwrite(buf, 1, size, *image_file);
}

void record_kernel_sectors(FILE **image_file, Elf32_Ehdr *task_ehdr, Elf32_Phdr *task_phdr)
{
	int kernel_size = 0x8000+task_phdr->p_filesz;  
	fseek(*image_file, 0x9c, SEEK_SET);   //need change!!!!!!!
	fwrite(&kernel_size, 4, 1, *image_file);
}


int main(int argc, char *argv[]){
	FILE *boot_file = NULL, *kernel_file = NULL, *image_file = NULL;
	FILE *pro1_file = NULL, *pro2_file=NULL;
	Elf32_Ehdr *boot_ehdr = NULL, *kernel_ehdr = NULL;
	Elf32_Ehdr *pro1_ehdr = NULL, *pro2_ehdr = NULL; 
	Elf32_Phdr *boot_phdr = NULL, *kernel_phdr = NULL;
	Elf32_Phdr *pro1_phdr = NULL, *pro2_phdr = NULL;
	


	image_file = fopen("image", "w");

	uint8_t boot_buf[4096];
        uint8_t kernel_buf[4096];
	uint8_t pro1_buf[4096];
	uint8_t pro2_buf[4096];
   

        boot_file = fopen("bootblock", "rb");
        kernel_file = fopen("kernel", "rb");
        pro1_file = fopen("process1","rb");
        pro2_file = fopen("process2","rb");

        fread(boot_buf, 4096, 1, boot_file);
        fread(kernel_buf, 4096, 1, kernel_file);    
        fread(pro1_buf, 4096, 1, pro1_file);
        fread(pro2_buf, 4096, 1, pro2_file);

        boot_ehdr = (void*) boot_buf;
        boot_phdr = (Elf32_Phdr*)((void*)boot_buf + boot_ehdr->e_phoff);
	kernel_ehdr = (void*) kernel_buf;
        kernel_phdr = (Elf32_Phdr*)((void*)kernel_buf + kernel_ehdr->e_phoff);
	pro1_ehdr = (void*) pro1_buf;
	pro1_phdr = (Elf32_Phdr*)((void*)pro1_buf + pro1_ehdr->e_phoff);
	pro2_ehdr = (void*) pro2_buf;
	pro2_phdr = (Elf32_Phdr*)((void*)pro2_buf + pro2_ehdr->e_phoff);

	write_bootblock(&image_file, boot_file, boot_ehdr, boot_phdr);
	write_kernel(&image_file, kernel_file, kernel_ehdr, kernel_phdr);   
	write_task(&image_file, pro1_file, pro1_ehdr, pro1_phdr, 0x7400);
	write_task(&image_file, pro2_file, pro2_ehdr, pro2_phdr, 0x8200);

	 //number of sector of kernel
 	//change bootblock
	record_kernel_sectors(&image_file, pro2_ehdr, pro2_phdr);

	printf("--------------------------IMAGE INFO--------------------------\n");
	printf("\tSIZE: %d\n",8200+pro2_phdr->p_filesz);
	printf("\tSEC NUMBER: %d\n",(8200+pro2_phdr->p_filesz)/512+(((8200+pro2_phdr->p_filesz)%512)!=0));

	printf("--------------------------KERNEL INFO--------------------------\n");
	printf("\tSIZE: %d\n",kernel_phdr->p_filesz);
	printf("\tSEC NUMBER: %d\n",(kernel_phdr->p_filesz)/512+(kernel_phdr->p_filesz % 512 != 0));   //////////
	printf("\tOFFSET in file: %d\n",kernel_phdr->p_offset);
	printf("\tOFFSET in memory: 0x%x\n",kernel_phdr->p_vaddr);
	printf("\tSIZE in file: %d\n",kernel_phdr->p_filesz);
	printf("\tSIZE in memory: %d\n",kernel_phdr->p_memsz);
	printf("\tALIGN SIZE: %d\n", kernel_phdr->p_align);

	printf("--------------------------BOOTBLOCK INFO--------------------------\n");
	printf("\tSIZE: %d\n",boot_phdr->p_filesz);
	printf("\tSEC NUMBER: %d\n",(boot_phdr->p_filesz)/512+(boot_phdr->p_filesz % 512 != 0));   //////////
	printf("\tOFFSET in file: %d\n",boot_phdr->p_offset);
	printf("\tOFFSET in memory: 0x%x\n",boot_phdr->p_vaddr);
	printf("\tSIZE in file:  %d\n",boot_phdr->p_filesz);
	printf("\tSIZE in memory: %d\n",boot_phdr->p_memsz);
	printf("\tALIGN SIZE: %d\n", boot_phdr->p_align);

	printf("--------------------------PROCESS1 INFO--------------------------\n");
	printf("\tSIZE: %d\n",pro1_phdr->p_filesz);
	printf("\tSEC NUMBER: %d\n",(pro1_phdr->p_filesz)/512+(pro1_phdr->p_filesz % 512 != 0));   //////////
	printf("\tOFFSET in file: %d\n",pro1_phdr->p_offset);
	printf("\tOFFSET in memory: 0x%x\n",pro1_phdr->p_vaddr);
	printf("\tSIZE in file:  %d\n",pro1_phdr->p_filesz);
	printf("\tSIZE in memory: %d\n",pro1_phdr->p_memsz);
	printf("\tsize of align is %d\n", pro1_phdr->p_align);

	printf("--------------------------PROCESS2 INFO--------------------------\n");
	printf("\tSIZE: %d\n",pro2_phdr->p_filesz);
	printf("\tSEC NUMBER: %d\n",(pro2_phdr->p_filesz)/512+(pro2_phdr->p_filesz % 512 != 0));   //////////
	printf("\tOFFSET in file: %d\n",pro2_phdr->p_offset);
	printf("\tOFFSET in memory: 0x%x\n",pro2_phdr->p_vaddr);
	printf("\tSIZE in file:  %d\n",pro2_phdr->p_filesz);
	printf("\tSIZE in memory: %d\n",pro2_phdr->p_memsz);
	printf("\tALIGN SIZE: %d\n", pro2_phdr->p_align);

	
	
	fclose(boot_file);
	fclose(kernel_file);
	fclose(image_file);
	fclose(pro1_file);
	fclose(pro2_file);
	return 0;
}
