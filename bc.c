#include "ext2.h"
#define BLK 1024

typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

unsigned short NSEC = 2;
char buf1[BLK], buf2[BLK];

int prints(char *s)
{
    while(*s != '\0') {
        putc(*s++);
    }
}

int gets(char s[ ])
{
    while((*s=getc()) != '\r') putc(*s++);
    *s = 0;
}

int getblk(u16 blk, char *buf) {
    readfd(blk/18, ((2*blk)%36)/18, ((2*blk)%36)%18, buf);
}

u16 search(INODE *ip, char *name) {
    int i;
    char c;
    DIR *dp;
    for (i = 0; i < 12; i++) {
        if ( (u16)ip->i_block[i] ) {
            getblk((u16)ip->i_block[i], buf2);
            dp = (DIR *)buf2;
            while( (char*) dp < &buf2[BLK]) {
                c = dp->name[dp->name_len];
                dp->name[dp->name_len] = 0;
                prints(dp->name);
                putc(' ');
                if ( strcmp(dp->name, name) == 0 ) {
                    prints("File found");
                    return ((u16) dp->inode);
                }
                dp->name[dp->name_len] = c;
                dp = (char *)dp + dp->rec_len;
            }
        }
        error();
    }
}

main() {
    char *cp, *name[2], filename[64];
    u16 i, ino, blk, iblk;
    u32 *up;
    GD *gp;
    INODE *ip;
    DIR *dp;
    name[0] = "boot"; name[1] = filename;
    prints("boot file: ");
    gets(filename);
    if (filename[0] == 0) name[1] = "mtx";
    getblk(2, buf1);
    gp = (GD *) buf1;
    iblk = (u16)gp->bg_inode_table;
    getblk(iblk, buf1);
    ip = (INODE *)buf1 + 1;
    for (i = 0; i < 2; i++) {
        ino = search(ip, name[i]) - 1;
        if (ino < 0) error();
        getblk(iblk + (ino/8), buf1);
        ip = (INODE*)buf1 + (ino % 8);
    }
    if ((u16)ip->i_block[12])
        getblk((u16)ip->i_block[12], buf2);
    setes(0x1000);
    for (i=0; i<12; i++) {
        getblk((u16) ip->i_block[i], 0);
        inces(); putc('*');
    }
    if ((u16) ip->i_block[12]) {
        up = (u32 *)buf2;
        while(*up++) {
            getblk((u16) *up, 0);
            inces(); putc('.');
        }
    }
    prints("ready to go");
    return 1;
}
