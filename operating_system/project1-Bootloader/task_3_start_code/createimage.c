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

int count_kernel_sectors(Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr) {
	return (kernel_phdr->p_filesz)/0x200 + 1;
}

void record_kernel_sectors(FILE **image_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr, int num_sec){
	fseek(*image_file, 0x40, SEEK_SET);   
	int inst = 0x34060000|kernel_phdr->p_filesz;
	fwrite(&inst, 4, 1, *image_file);
}

void extended_opt(Elf32_Phdr *boot_phdr, int k_phnum, Elf32_Phdr *kernel_phdr, int num_sec){
	//image information
	printf("Image info:\n");
	printf("\tsize of image is %d\n",512*(num_sec+1));
	printf("\tnumber of image sectors is %d\n",num_sec+1);

	//kernel information
	printf("Kernel info:\n");
	printf("\tsize of kernel is %d\n",kernel_phdr->p_filesz);
	printf("\tnumber of sector is 1\n");
	printf("\toffset of segment in file is %d\n",kernel_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",kernel_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",kernel_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",kernel_phdr->p_memsz);
	printf("\tsize of align is %d\n", kernel_phdr->p_align);

	//boot infomation
	printf("Boot info:\n");
	printf("\tsize of boot is %d\n",boot_phdr->p_filesz);
	printf("\tnumber of sector is %d\n",num_sec);
	printf("\toffset of segment in file is %d\n",boot_phdr->p_offset);
	printf("\toffset of segment in memory is 0x%x\n",boot_phdr->p_vaddr);
	printf("\tsize of segment in file is %d\n",boot_phdr->p_filesz);
	printf("\tsize of segment in memory is %d\n",boot_phdr->p_memsz);
	printf("\tsize of align is %d\n", boot_phdr->p_align);
}

int main(int argc, char *argv[]){
	FILE *boot_file = NULL, *kernel_file = NULL, *image_file = NULL;
	Elf32_Phdr *boot_phdr = NULL, *kernel_phdr = NULL;
	Elf32_Ehdr *boot_ehdr = NULL, *kernel_ehdr = NULL;
	int num_sec = 0, k_phnum = 0;

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

	//write boot and kernel to image
	write_bootblock(&image_file, boot_file, boot_ehdr, boot_phdr);
	write_kernel(&image_file, kernel_file, kernel_ehdr, kernel_phdr);   

 	//change bootblock
        num_sec = (kernel_phdr->p_filesz)/0x200 + 1;
	record_kernel_sectors(&image_file, kernel_ehdr, kernel_phdr, num_sec);

	//printf info
	k_phnum = kernel_ehdr->e_phnum;
	extended_opt(boot_phdr, k_phnum, kernel_phdr, num_sec);

	fclose(boot_file);
	fclose(kernel_file);
	fclose(image_file);
	return 0;
}
