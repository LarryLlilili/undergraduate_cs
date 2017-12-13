bootblock.s汇编文件将内核加载到内存。
createimage.c为生成内核镜像的工具,它读取image, kernel, bootblock三个可执行文件，并把kernel 和 bootblock写入image中，最后打印出kernel和bootblock文件的相关信息。
