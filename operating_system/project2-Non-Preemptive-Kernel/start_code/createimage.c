#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_bootblock(FILE **image_file, FILE *boot_file, Elf32_Ehdr *boot_ehar, Elf32_Phdr *boot_phdr) {
	Elf32_Word size = boot_phdr->p_filesz;
	uint8_t buf[size];
	fseek(boot_file, boot_phdr->p_offset, SEEK_SET);   
	fread(buf, size, 1, boot_file);
	fwrite(buf, 1, size, *image_file);
}

void write_kernel(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr) {
	Elf32_Word size = kernel_phdr->p_filesz;
	uint8_t buf[size];
	fseek(kernel_file, kernel_phdr->p_offset, SEEK_SET);    
	fread(buf, size, 1, kernel_file);
	fseek(*image_file, 512, SEEK_SET);
	fwrite(buf, 1, size, *image_file);
}

void write_task(FILE **image_file, FILE *task_file, Elf32_Ehdr *task_ehdr, Elf32_Phdr *task_phdr, uint32_t addr){
	Elf32_Word size=task_phdr->p_filesz;
	uint8_t buf[size];
	fseek(task_file, task_phdr->p_offset, SEEK_SET);
	fread(buf, size, 1, task_file);
	fseek(*image_file, addr, SEEK_SET);  // 128*512 Bytes
	fwrite(buf, 1, size, *image_file);
}

void record_kernel_sectors(FILE **image_file, Elf32_Ehdr *task_ehdr, Elf32_Phdr *task_phdr, int num_sec){
	fseek(*image_file, 0x70, SEEK_SET);   
	int ker_size = 127*0x200+128*2*0x200+task_phdr->p_filesz;  
	fwrite(&ker_size, 4, 1, *image_file);
}


int main(int argc, char *argv[]){
	FILE *boot_file = NULL, *kernel_file = NULL, *image_file = NULL, *pro1_file = NULL, *pro2_file=NULL, *pro3_file=NULL;
	Elf32_Phdr *boot_phdr = NULL, *kernel_phdr = NULL, *pro1_phdr=NULL, *pro2_phdr=NULL,*pro3_phdr;
	Elf32_Ehdr *boot_ehdr = NULL, *kernel_ehdr = NULL, *pro1_ehdr=NULL, *pro2_ehdr=NULL, *pro3_ehdr;
	int num_sec = 0;

	image_file = fopen("image", "w");

	//read kernel and bootblock
        uint8_t boot_buf[4096];
        boot_file = fopen("bootblock", "rb");
        fread(boot_buf, 4096, 1, boot_file);
        boot_ehdr = (void*) boot_buf;
        boot_phdr = (Elf32_Phdr*)((void*)boot_buf + boot_ehdr->e_phoff);
        uint8_t kernel_buf[4096];
        kernel_file = fopen("kernel", "rb");
        fread(kernel_buf, 4096, 1, kernel_file);
        kernel_ehdr = (void*) kernel_buf;
        kernel_phdr = (Elf32_Phdr*)((void*)kernel_buf + kernel_ehdr->e_phoff);

	uint8_t pro1_buf[4096];
	pro1_file = fopen("process1","rb");
	fread(pro1_buf, 4096, 1, pro1_file);
	pro1_ehdr = (void*) pro1_buf;
	pro1_phdr = (Elf32_Phdr*)((void*)pro1_buf + pro1_ehdr->e_phoff);

	uint8_t pro2_buf[4096];
	pro2_file = fopen("process2","rb");
	fread(pro2_buf, 4096, 1, pro2_file);
	pro2_ehdr = (void*) pro2_buf;
	pro2_phdr = (Elf32_Phdr*)((void*)pro2_buf + pro2_ehdr->e_phoff);
	
	uint8_t pro3_buf[4096];
	pro3_file = fopen("process3","rb");
	fread(pro3_buf, 4096, 1, pro3_file);
	pro3_ehdr = (void*) pro3_buf;
	pro3_phdr = (Elf32_Phdr*)((void*)pro3_buf + pro3_ehdr->e_phoff);

	//write boot and kernel to image
	write_bootblock(&image_file, boot_file, boot_ehdr, boot_phdr);
	write_kernel(&image_file, kernel_file, kernel_ehdr, kernel_phdr);   

        num_sec = 4;  //number of sector of kernel

	write_task(&image_file, pro1_file, pro1_ehdr, pro1_phdr, 0x10000);
	write_task(&image_file, pro2_file, pro2_ehdr, pro2_phdr, 0x20000);
	write_task(&image_file, pro3_file, pro3_ehdr, pro3_phdr, 0x30000);
	
 	//change bootblock
	record_kernel_sectors(&image_file, pro3_ehdr, pro3_phdr, num_sec);


	//print image information
	printf("Image info:\n");
	printf("\tsize of image is %d\n",512*128*3+pro3_phdr->p_filesz);
	printf("\tnumber of image sectors is 4\n");

	//print kernel information
	printf("Kernel info:\n");
	printf("\tsize of kernel is %d\n",kernel_phdr->p_filesz);
	printf("\tnumber of sector is %d\n",(kernel_phdr->p_filesz)/512+(kernel_phdr->p_filesz % 512 != 0));   //////////
	printf("\toffset of segment in file is %d\n",kernel_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",kernel_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",kernel_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",kernel_phdr->p_memsz);
	printf("\tsize of align is %d\n", kernel_phdr->p_align);

	//print boot infomation
	printf("boot info:\n");
	printf("\tsize of boot is %d\n",boot_phdr->p_filesz);
	printf("\tnumber of sector is %d\n",(boot_phdr->p_filesz)/512+(boot_phdr->p_filesz % 512 != 0));   //////////
	printf("\toffset of segment in file is %d\n",boot_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",boot_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",boot_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",boot_phdr->p_memsz);
	printf("\tsize of align is %d\n", boot_phdr->p_align);


	//print pro1 info
	printf("Process1 info:\n");
	printf("\tsize of process1 is %d\n",pro1_phdr->p_filesz);
	printf("\tnumber of sector is %d\n",(pro1_phdr->p_filesz)/512+(pro1_phdr->p_filesz % 512 != 0));   //////////
	printf("\toffset of segment in file is %d\n",pro1_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",pro1_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",pro1_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",pro1_phdr->p_memsz);
	printf("\tsize of align is %d\n", pro1_phdr->p_align);

	//print pro2 info
	printf("Process2 info:\n");
	printf("\tsize of process2 is %d\n",pro2_phdr->p_filesz);
	printf("\tnumber of sector is %d\n",(pro2_phdr->p_filesz)/512+(pro2_phdr->p_filesz % 512 != 0));   //////////
	printf("\toffset of segment in file is %d\n",pro2_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",pro2_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",pro2_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",pro2_phdr->p_memsz);
	printf("\tsize of align is %d\n", pro2_phdr->p_align);

	//print pro3 infob 
	printf("Process3 info:\n");
	printf("\tsize of process3 is %d\n",pro3_phdr->p_filesz);
	printf("\tnumber of sector is %d\n",(pro3_phdr->p_filesz)/512+(pro3_phdr->p_filesz % 512 != 0));   //////////
	printf("\toffset of segment in file is %d\n",pro3_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",pro3_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",pro3_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",pro3_phdr->p_memsz);
	printf("\tsize of align is %d\n", pro3_phdr->p_align);
	
	fclose(boot_file);
	fclose(kernel_file);
	fclose(image_file);
	return 0;
}
