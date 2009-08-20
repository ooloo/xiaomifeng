#include <assert.h>
#include <stdio.h>
#include <math.h>

int main(int argc,char **argv)
{
	int i;
	unsigned a,b,c,d,count,region;
	//unsigned v,w,x,y,z;
	char *t,src[128];
	FILE *fp;
	
	if(argc !=2)
		exit(1);

	fp=fopen(argv[1],"r");
	assert(fp);

	while(fgets(src,128,fp)!=NULL)
	{
		t=src;
		while(*t)
		{
			if(*t == '/' || *t == '.')
				*t = ' ';
			t++;
		}
		
		sscanf(src,"%u %u %u %u %u %u\n",&a,&b,&c,&d,&count,&region);
		assert(count >= 8);

		if(count < 16)
		{
			for(i=0; i < (int)pow(2.0,(16-count)); i++)
			{
				printf("%u.%u.%u.%u/%u %u\n",a,b,c,d,16,region);
				assert(++b <= 256);
			}
		}
		else
			printf("%u.%u.%u.%u/%u %u\n",a,b,c,d,count,region);
		
		//z=(((a*256+b)*256+c)*256+d);
		//z+=count-1;
		//y=z%256; z/=256;
		//x=z%256; z/=256;
		//w=z%256; z/=256;
		//v=z%256; z/=256;
	}

	fclose(fp);
}
