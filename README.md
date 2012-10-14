
存储分配程序

先分配一块大的。然后在这个大内存上面做操作

一个链表来维护一个not used内存表

节点的数据结构    
typedef struct _node{    
    char *address;    
	int length;    
	_node *nextFreeMem;    
}FreeMemNode;     
address表示not used内存块的首地址     
length表示not used内存块的长度    
nextFreeMem表示指向下一个记录not used内存的节点    


申请时，可选择策略(first fit or best fit)，从not used链表找到内存分配。   
实际申请的内存会多4个字节，用来存储所申请的内存大小。     
没有内存可申请就返回NULL。   

释放时，把这段该释放的内存通过维护一个FreeMemNode节点，加入到not used链表里面，节点按照address值递增排序。    
加入链表时，如果前后节点记录的address能够合并，就把这两个节点合并。   
