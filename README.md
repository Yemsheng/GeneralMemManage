
�洢�������

�ȷ���һ���ġ�Ȼ����������ڴ�����������

һ��������ά��һ��not used�ڴ��

�ڵ�����ݽṹ    
typedef struct _node{    
    char *address;    
	int length;    
	_node *nextFreeMem;    
}FreeMemNode;     
address��ʾnot used�ڴ����׵�ַ     
length��ʾnot used�ڴ��ĳ���    
nextFreeMem��ʾָ����һ����¼not used�ڴ�Ľڵ�    


����ʱ����ѡ�����(first fit or best fit)����not used�����ҵ��ڴ���䡣   
ʵ��������ڴ���4���ֽڣ������洢��������ڴ��С��     
û���ڴ������ͷ���NULL��   

�ͷ�ʱ������θ��ͷŵ��ڴ�ͨ��ά��һ��FreeMemNode�ڵ㣬���뵽not used�������棬�ڵ㰴��addressֵ��������    
��������ʱ�����ǰ��ڵ��¼��address�ܹ��ϲ����Ͱ��������ڵ�ϲ���   
