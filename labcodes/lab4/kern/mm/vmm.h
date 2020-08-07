#ifndef __KERN_MM_VMM_H__
#define __KERN_MM_VMM_H__

#include <defs.h>
#include <list.h>
#include <memlayout.h>
#include <sync.h>

//pre define
struct mm_struct;

// the virtual continuous memory area(vma), [vm_start, vm_end), 
// addr belong to a vma means  vma.vm_start<= addr <vma.vm_end 
struct vma_struct {
    struct mm_struct *vm_mm; // the set of vma using the same PDT 一个指针，指向一个比vma_struct更高等级的抽象数据结构mm_struct，包含所有虚拟内存空间的共同属性
    uintptr_t vm_start;      // start addr of vma      
    uintptr_t vm_end;        // end addr of vma, not include the vm_end itself [vm_start, vm_end),描述地址空间范围
    uint32_t vm_flags;       // flags of vma  虚拟内存空间的属性
    list_entry_t list_link;  // linear list link which sorted by start addr of vma  list_entry，双向链表，将一系列vma连接起来
};

#define le2vma(le, member)                  \
    to_struct((le), struct vma_struct, member)

#define VM_READ                 0x00000001  // 只读
#define VM_WRITE                0x00000002  // 可读写
#define VM_EXEC                 0x00000004  // 可执行

// the control struct for a set of vma using the same PDT
struct mm_struct {
    list_entry_t mmap_list;        // linear list link which sorted by start addr of vma  // 双向链表头，链接了所有属于同一页目录表的虚拟内存空间
    struct vma_struct *mmap_cache; // current accessed vma, used for speed purpose  // 当前正在使用的虚拟内存空间
    pde_t *pgdir;                  // the PDT of these vma  //mm_struct数据结构维护的页表的首地址
    int map_count;                 // the count of these vma  //mmap_list中vma_struct结构的个数
    void *sm_priv;                   // the private data for swap manager  // 链接记录页访问情况的链表头
};

struct vma_struct *find_vma(struct mm_struct *mm, uintptr_t addr);
struct vma_struct *vma_create(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags);
void insert_vma_struct(struct mm_struct *mm, struct vma_struct *vma);

struct mm_struct *mm_create(void);
void mm_destroy(struct mm_struct *mm);

void vmm_init(void);

int do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr);

extern volatile unsigned int pgfault_num;
extern struct mm_struct *check_mm_struct;
#endif /* !__KERN_MM_VMM_H__ */

