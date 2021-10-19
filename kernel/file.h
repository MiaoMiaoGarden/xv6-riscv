struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;  // type字段区分了文件、管道、设备等，type==0表示该node是空闲的
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define	mkdev(m,n)  ((uint)((m)<<16| (n)))

// in-memory copy of an inode
// 内核将正在使用的inode保存在文件中，结构体inode是磁盘dinode的拷贝
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count  指向inode的指针的数量， 如果ref==0，则内核从内存中丢弃这个inode
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;        // 统计链接这个inode的目录项的数量， copy of disk inode
  uint size;          // 文件中内容的字节数
  uint addrs[NDIRECT+1];   // 记录了持有文件内容的磁盘块的块号
};

// map major device number to device functions.
struct devsw {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
