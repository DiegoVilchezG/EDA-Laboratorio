#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct QuadtreeNode{
	unsigned char blue, green, red, reserved;
	uint32_t area;
	int32_t bottom_left, top_left;
	int32_t bottom_right, top_right;
} __attribute__((packed)) QuadtreeNode;

typedef struct FileHeader{
	short signature;
	int file_size, reserved, offset;
} FileHeader;

typedef struct InfoHeader{
	int size;
	int width, height;
	short planes, bit_count;
	int compression, image_size, x_pixels_per_meter, y_pixels_per_meter, colors_used, colors_important;
} InfoHeader;

typedef struct pixel{
	unsigned char blue, green, red, reserved;
} pixel;

typedef struct tree{
	QuadtreeNode *nod;
	struct tree *left1, *left2, *right1, *right2;
} tree;

typedef struct list{
	tree *val;
	struct list *next;
} list;

int isUniform(pixel **a, int l, int r, int u, int d)
{
	int i,j;
	pixel *aux,*aux2;
	for(i=u;i<d;i++)
	{
		for(j=l;j<r;j++)
		{
			if(i==u && j==l)
				aux=&a[i][j];
			aux2=&a[i][j];
			if(!(aux->blue==aux2->blue && aux->green==aux2->green && aux->red==aux2->red && aux->reserved==aux2->reserved))
			{
				return 0;
			}
			
			aux=&a[i][j];
		}
	}
	return 1;
}

void init(tree **head)
{
	*head=NULL;
}

uint32_t countFrunze(tree *head)
{
	uint32_t k=0;
	if(head->left1==NULL && head->left2==NULL && head->right1==NULL && head->right2==NULL)
		return 1;
	else
	{
		k=countFrunze(head->left1)+countFrunze(head->left2)+countFrunze(head->right1)+countFrunze(head->right2);
		return k;
	}
}

uint32_t countNoduri(tree *head)
{
	uint32_t k=0;
	if(head==NULL)
		return 0;
	k=1+countNoduri(head->left1)+countNoduri(head->left2)+countNoduri(head->right1)+countNoduri(head->right2);
	return k;
}

void initL(list **head)
{
	*head=NULL;
}

void insertL(list **head, tree *val)
{
	list *new=malloc(sizeof(list));
	new->next=NULL;
	new->val=val;
	if(*head==NULL)
		*head=new;
	else
	{
		list *aux=*head;
		while(aux->next)
			aux=aux->next;
		aux->next=new;
	}
}

void freeL(list **head)
{
	list *aux=*head;
	while(*head)
	{
		*head=(*head)->next;
		free(aux);
		aux=(*head);
	}
}

tree *pop(list **head)
{
	if(*head)
	{
		list *aux=*head;
		*head=(*head)->next;
		return aux->val;
	}
	else
		return NULL;
}

void freetree(tree **root)
{
	tree *aux;
	list *head;
	initL(&head);
	insertL(&head,(*root));
	while(head)
	{
		aux=pop(&head);
		if(aux!=NULL)
		{
			if(aux->left1!=NULL)
		        insertL(&head, aux->left1);
			if(aux->left2!=NULL)
		        insertL(&head, aux->left2);
		        if(aux->right1!=NULL)
		        insertL(&head, aux->right1);
			if(aux->right2!=NULL)
		        insertL(&head, aux->right2);
			free(aux);
			aux=NULL;
		}
	}
	*root=NULL;
	freeL(&head);
}

tree **bfs(tree *root, list *head, uint32_t numar_noduri)
{
	tree *aux;
	tree **v=malloc(numar_noduri*sizeof(tree*));
	int i=0;
	insertL(&head,root);
	while(head)
	{
		aux=pop(&head);
		v[i++]=aux;
	        if(aux->left1!=NULL)
	        insertL(&head, aux->left1);
		if(aux->left2!=NULL)
	        insertL(&head, aux->left2);
	        if(aux->right1!=NULL)
	        insertL(&head, aux->right1);
		if(aux->right2!=NULL)
	        insertL(&head, aux->right2);
	}
	freeL(&head);
	return v;
}

QuadtreeNode *indexing(tree **v, uint32_t numar_noduri)
{
	uint32_t i,j;
	for(i=0;i<numar_noduri-1;i++)
	{
		for(j=i+1;j<numar_noduri;j++)
		{
			if(v[i]->left1==v[j])
				v[i]->nod->top_left=j;
			if(v[i]->left2==v[j])
				v[i]->nod->top_right=j;
			if(v[i]->right1==v[j])
				v[i]->nod->bottom_right=j;
			if(v[i]->right2==v[j])
				v[i]->nod->bottom_left=j;
		}
	}
	QuadtreeNode *vector=malloc(numar_noduri*sizeof(QuadtreeNode));
	for(i=0;i<numar_noduri;i++)
	{
		vector[i].blue=v[i]->nod->blue;
		vector[i].green=v[i]->nod->green;
		vector[i].red=v[i]->nod->red;
		vector[i].reserved=v[i]->nod->reserved;
		vector[i].area=v[i]->nod->area;
		vector[i].top_left=v[i]->nod->top_left;
		vector[i].top_right=v[i]->nod->top_right;
		vector[i].bottom_right=v[i]->nod->bottom_right;
		vector[i].bottom_left=v[i]->nod->bottom_left;
		free(v[i]->nod);
		free(v[i]);
	}
	free(v);
	return vector;
}

tree *ceva(pixel **a, int l, int r, int u, int d)
{
	tree *new=malloc(sizeof(tree));
	new->left1=NULL;
	new->left2=NULL;
	new->right1=NULL;
	new->right2=NULL;
	new->nod=malloc(sizeof(QuadtreeNode));
	new->nod->blue=0;
	new->nod->green=0;
	new->nod->red=0;
	new->nod->reserved=0;
	new->nod->area=(r-l)*(r-l);
	new->nod->bottom_left=-1;
	new->nod->bottom_right=-1;
	new->nod->top_left=-1;
	new->nod->top_right=-1;
	if(isUniform(a,l,r,u,d))
	{
		new->nod->blue=a[u][l].blue;
		new->nod->green=a[u][l].green;
		new->nod->red=a[u][l].red;
		new->nod->reserved=a[u][l].reserved;
		new->left1=NULL;
		new->left2=NULL;
		new->right1=NULL;
		new->right2=NULL;
		new->nod->bottom_left=-1;
		new->nod->bottom_right=-1;
		new->nod->top_left=-1;
		new->nod->top_right=-1;
		return new;
	}
	else
	{
		new->left1=ceva(a,l,(l+r)/2,u,(u+d)/2);
		new->left2=ceva(a,(l+r)/2,r,u,(u+d)/2);
		new->right1=ceva(a,(l+r)/2,r,(u+d)/2,d);
		new->right2=ceva(a,l,(l+r)/2,(u+d)/2,d);
		return new;
	}
}

int main(int argc, char *argv[])
{
	FileHeader *file_header=malloc(sizeof(FileHeader));
	InfoHeader *info_header=malloc(sizeof(InfoHeader));
	FILE *file=fopen(argv[argc-2],"rb");
	FILE *out=fopen(argv[argc-1],"wb");
	
	fread(&file_header->signature, sizeof(short), 1, file);
	fread(&file_header->file_size, sizeof(int), 1, file);
	fread(&file_header->reserved, sizeof(int), 1, file);
	fread(&file_header->offset, sizeof(int), 1, file);


	fread(&info_header->size, sizeof(int), 1, file);
	fread(&info_header->width, sizeof(int), 1, file);
	fread(&info_header->height, sizeof(int), 1, file);
	fread(&info_header->planes, sizeof(short), 1, file);
	fread(&info_header->bit_count, sizeof(short), 1, file);
	fread(&info_header->compression, sizeof(int), 1, file);
	fread(&info_header->image_size, sizeof(int), 1, file);
	fread(&info_header->x_pixels_per_meter, sizeof(int), 1, file);
	fread(&info_header->y_pixels_per_meter, sizeof(int), 1, file);
	fread(&info_header->colors_used, sizeof(int), 1, file);
	fread(&info_header->colors_important, sizeof(int), 1, file);


	fwrite(&file_header->signature, sizeof(short), 1, out);
	fwrite(&file_header->file_size, sizeof(int), 1, out);
	fwrite(&file_header->reserved, sizeof(int), 1, out);
	fwrite(&file_header->offset, sizeof(int), 1, out);


	fwrite(&info_header->size, sizeof(int), 1, out);
	fwrite(&info_header->width, sizeof(int), 1, out);
	fwrite(&info_header->height, sizeof(int), 1, out);
	fwrite(&info_header->planes, sizeof(short), 1, out);
	fwrite(&info_header->bit_count, sizeof(short), 1, out);
	fwrite(&info_header->compression, sizeof(int), 1, out);
	fwrite(&info_header->image_size, sizeof(int), 1, out);
	fwrite(&info_header->x_pixels_per_meter, sizeof(int), 1, out);
	fwrite(&info_header->y_pixels_per_meter, sizeof(int), 1, out);
	fwrite(&info_header->colors_used, sizeof(int), 1, out);
	fwrite(&info_header->colors_important, sizeof(int), 1, out);


	if(strcmp(argv[1],"-c")==0)
	{
		
		int i,j;
		pixel **a=malloc((info_header->height)*sizeof(pixel*));
		for(i=info_header->height-1;i>=0;i--)
		{
			a[i]=malloc((info_header->width)*sizeof(pixel));
			for(j=0;j<info_header->width;j++)
			{
				fread(&a[i][j], sizeof(pixel), 1, file);
			}
		}

		uint32_t numar_culori, numar_noduri;
		tree *head;
		head=ceva(a, 0, info_header->width, 0, info_header->height);
		numar_culori=countFrunze(head);
		numar_noduri=countNoduri(head);


		list *lista;
		initL(&lista);
		tree **v=bfs(head, lista, numar_noduri);
		QuadtreeNode *vector=indexing(v, numar_noduri);


		fwrite(&numar_culori, sizeof(uint32_t), 1, out);
		fwrite(&numar_noduri, sizeof(uint32_t), 1, out);
		uint32_t k;
		for(k=0;k<numar_noduri;k++)
		{
			fwrite(&vector[k], sizeof(QuadtreeNode), 1, out);
		}

		free(vector);
		freeL(&lista);
		for(i=info_header->height-1;i>=0;i--)
		{
			free(a[i]);
		}
		free(a);

	}

	if(strcmp(argv[1],"-d")==0)
	{
		uint32_t numar_noduri, numar_culori, k;
		fread(&numar_culori, sizeof(uint32_t), 1, file);
		fread(&numar_noduri, sizeof(uint32_t), 1, file);
		QuadtreeNode *vector = malloc((numar_noduri) * sizeof(QuadtreeNode));
		for(k=0;k<numar_noduri;k++)
		{
			fread(&vector[k], sizeof(QuadtreeNode), 1, file);
		}
	}
	free(file_header);
	free(info_header);
	fclose(file);
	fclose(out);
	return 0;
}
