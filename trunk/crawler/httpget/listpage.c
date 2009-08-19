#include <stdlib.h>
#include <string.h>

char *memmem(const char *str1, const char *str1End, const char *str2, int str2Len)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	const char *s1End, *s2End;

	if (!str2 || str2Len <= 0)
		return((char *)str1);
	if (str2Len > (int)(str1End - str1))
		return(NULL);

	s1End = str1End - str2Len + 1;
	s2End = str2 + str2Len;

	while (cp < s1End)
	{
		s1 = cp;
		s2 = (char *) str2;

		while (s2 != s2End && *s1 == *s2)
			s1++, s2++;

		if (s2 == s2End)
			return(cp);

		cp++;
	}

	return(NULL);

}

/*Gets the minimum of three values*/
int minimum(int a,int b,int c)
{
	int min=a;
	if(b<min)
		min=b;
	if(c<min)
		min=c;
	return min;
}

// ������С�༭���룬n��m��ʾs��t�ĳ���
/*Compute levenshtein distance between s and t*/
int levenshtein_distance(const char *s, int n, const char*t, int m)
{
	//Step 1
	int k,i,j,cost,*d,distance;
	if(n!=0&&m!=0)
	{
		d=malloc((sizeof(int))*(m+1)*(n+1));
		m++;
		n++;
		//Step 2	
		for(k=0;k<n;k++)
			d[k]=k;
		for(k=0;k<m;k++)
			d[k*n]=k;
		//Step 3 and 4	
		for(i=1;i<n;i++)
			for(j=1;j<m;j++)
			{
				//Step 5
				if(s[i-1]==t[j-1])
					cost=0;
				else
					cost=1;
				//Step 6			 
				d[j*n+i]=minimum(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
			}
		distance=d[n*m-1];
		free(d);
		return distance;
	}
	else 
		return -1; //a negative return value means that one or both strings are empty.
}

#define SIMILAR_LONG_SECTION		13		// �Խϳ���������Ϊ13��
#define SIMILAR_SHORT_SECTION		11		// �Խ϶̵�������Ϊ11��
#define SIMILAR_SMALL_ELEMENT_LEN	10		// �Ƚ϶�Ԫ�صĳ���
#define SIMILAR_MID_ELEMENT_LEN		40		// �Ƚ���Ԫ�صĳ���
#define SIMILAR_BIG_ELEMENT_LEN		80		// �Ƚϳ�Ԫ�صĳ���

// ����������ҳ�����ƶ�
int get_web_page_similarity(const char *characterA, int lengthA, const char *characterB, int lengthB)
{
	const char *innerA, *innerB;		// �ڲ���A��B������AΪmemcmp��С���Ǹ�
	int innerLenA, innerLenB;
	int minLen, maxLen;					// A��B����С���Ⱥ���󳤶�
	int sameLen;						// A,B��ʼ�ͽ�β����ͬ�ĳ���
	const char *tailA, *tailB;
	int tailLen;						// ��β����ͬ�ĳ���
	int sectionsA, sectionsB;			// A,B�ķֶ���
	int elementLen;						// ѡ���ıȽ�Ԫ�صĳ���
	int stepA, stepB;					// A,B��ÿ���ֶεĲ�������
	const char *element;				// ��Ҫƥ���Ԫ��
	const char *lastPosA, *lastPosB;	// A,B�ϸ�ƥ�䵽Ԫ�ص�λ��
	const char *curPosA, *curPosB;		// A,B����ƥ���Ԫ�ص�λ��
	const char *searchBegin, *searchEnd;	// ��Ҫ����element��Ŀ�괮��Χ
	int matchLenA, matchLenB;			// A,B��ǰƥ�䵽Ԫ�غ��ϸ�ƥ�䵽Ԫ��֮��ľ���
	int i;
	int similarity;						// ���ƶ�

	minLen = lengthA > lengthB ? lengthB : lengthA;
	maxLen = lengthA > lengthB ? lengthA : lengthB;
	if (minLen <= 0 || characterA == NULL || characterB == NULL)
		return -1;

	// ȷ��innerA��innerB����ȷ����ͷ��ͬ�ĳ���
	innerA = characterA;
	innerB = characterB;
	i = minLen;
	while (i > 0 && *innerA == *innerB )
	{
		innerA++;
		innerB++;
		i--;
	}
	// ��ʼ��ͬ�ĳ���
	sameLen = minLen - i;

	// ȷ��innerA��innerB���Լ����ǵĳ���
	if (*innerA < *innerB)
	{
		innerLenA = lengthA - sameLen;
		innerLenB = lengthB - sameLen;
	}
	else if (*innerA > *innerB)
	{
		// ����innerA��innerB
		innerA = innerB;
		innerLenA = lengthB - sameLen;
		innerB = characterA + sameLen;
		innerLenB = lengthA - sameLen;
	}
	else
	{
		// �����϶̵��Ǹ���ϳ��Ǹ��Ŀ�ʼ����ȫ��ͬ
		return minLen * 100 / maxLen;
	}

	minLen = innerLenA > innerLenB ? innerLenB : innerLenA;
	maxLen = innerLenA > innerLenB ? innerLenA : innerLenB;

	// ȷ��β����ͬ�ĳ���
	i = minLen;
	tailA = innerA + innerLenA - 1;
	tailB = innerB + innerLenB - 1;
	while (i > 0 && *tailA == *tailB)
	{
		tailA--;
		tailB--;
		i--;
	}
	if (i == 0)
	{
		// �����϶̵��Ǹ���ϳ��Ǹ��Ŀ�ͷ�ͽ�β����ȫ��ͬ
		return (minLen + sameLen) * 100 / (maxLen + sameLen);
	}

	// ��β��ͬ�ĳ���
	tailLen = minLen - i;
	// sameLenΪ��β��ͬ�ܳ���
	sameLen += tailLen;

	// �˺�tailָ�����λ�ã������һ����Ч�ַ�����һ��λ��
	tailA++;
	tailB++;

	// �˺��A��B����ͷȥβ
	innerLenA -= tailLen;
	innerLenB -= tailLen;
	minLen -= tailLen;
	maxLen -= tailLen;

	similarity = minLen * 100 / maxLen;
	if (similarity < 50)
	{
		// ʣ�೤�����50%���ϣ������бȽ���
		similarity = (100 * sameLen + similarity * minLen) / (maxLen + sameLen);
		return similarity;
	}

	if (minLen < 4 * SIMILAR_SMALL_ELEMENT_LEN)
	{
		// ���ڱȽ϶̵�������ʹ�ñ༭����������ƶ�
		similarity = maxLen - levenshtein_distance(innerA, innerLenA, innerB, innerLenB);
		similarity = 100 * (sameLen + similarity) / (maxLen + sameLen);
		return similarity;
	}

	// ȷ��sectionsA��sectionB
	if (innerLenA > innerLenB)
	{
		sectionsA = SIMILAR_LONG_SECTION;
		sectionsB = SIMILAR_SHORT_SECTION;
	}
	else
	{
		sectionsB = SIMILAR_LONG_SECTION;
		sectionsA = SIMILAR_SHORT_SECTION;
	}

	// ȷ��ʹ��Ԫ�ش�С
	if (minLen > SIMILAR_LONG_SECTION * SIMILAR_BIG_ELEMENT_LEN * 8)
		elementLen = SIMILAR_BIG_ELEMENT_LEN;
	else if (minLen > SIMILAR_LONG_SECTION * SIMILAR_MID_ELEMENT_LEN * 8)
		elementLen = SIMILAR_MID_ELEMENT_LEN;
	else
		elementLen = SIMILAR_SMALL_ELEMENT_LEN;

	similarity = 0;

	// ��A���зֶ�
	stepA = (innerLenA - elementLen) / (sectionsA - 1);
	element = innerA;
	lastPosA = NULL;
	lastPosB = NULL;
	for (i = 0; i < sectionsA; i++)
	{
		curPosA = element;
		// ������ҿ�ʼ�ͽ���λ��
		if (lastPosA == NULL)
		{
			searchBegin = innerB;
			searchEnd = innerB + stepA * 2 + elementLen;
			if (searchEnd > tailB)
			{
				searchEnd = tailB;
			}
		}
		else
		{
			searchBegin = lastPosB + (curPosA - lastPosA) / 2;
			if (searchBegin > tailB - elementLen)
			{
				searchBegin = lastPosB;
				searchEnd = tailB;
			}
			else
			{
				searchEnd = lastPosB + (curPosA - lastPosA) * 2 + elementLen;
				if (searchEnd > tailB)
				{
					searchEnd = tailB;
				}
			}
		}

		// ����element
		curPosB = memmem(searchBegin, searchEnd, element, elementLen);
		if (curPosB)
		{
			// �ҵ�
			if (lastPosA == NULL)
			{
				// ƥ���һ��Ԫ��ʱ����Ҫ����elementLen
				matchLenA = (int)(curPosA - innerA) + elementLen;
				matchLenB = (int)(curPosB - innerB) + elementLen;
			}
			else
			{
				matchLenA = (int)(curPosA - lastPosA);
				matchLenB = (int)(curPosB - lastPosB);
			}
			if (matchLenB < matchLenA)
			{
				similarity += matchLenB * 100 / matchLenA;
			}
			else
			{
				similarity += matchLenA * 100 / matchLenB;
			}

			// ��¼�ϴ�ƥ�䵽��λ��
			lastPosA = curPosA;
			lastPosB = curPosB;
		}
		element += stepA;
	}

	// ��B���зֶ�
	stepB = (innerLenB - elementLen) / (sectionsB - 1);
	element = innerB;
	lastPosA = NULL;
	lastPosB = NULL;
	for (i = 0; i < sectionsB; i++)
	{
		curPosB = element;
		// ������ҿ�ʼ�ͽ���λ��
		if (lastPosB == NULL)
		{
			searchBegin = innerA;
			searchEnd = innerA + stepB * 2 + elementLen;
			if (searchEnd > tailA)
			{
				searchEnd = tailA;
			}
		}
		else
		{
			searchBegin = lastPosA + (curPosB - lastPosB) / 2;
			if (searchBegin > tailA - elementLen)
			{
				searchBegin = lastPosA;
				searchEnd = tailA;
			}
			else
			{
				searchEnd = lastPosA + (curPosB - lastPosB) * 2 + elementLen;
				if (searchEnd > tailA)
				{
					searchEnd = tailA;
				}
			}
		}

		// ����element
		curPosA = memmem(searchBegin, searchEnd, element, elementLen);
		if (curPosA)
		{
			// �ҵ�
			if (lastPosB == NULL)
			{
				// ƥ���һ��Ԫ��ʱ����Ҫ����elementLen
				matchLenA = (int)(curPosA - innerA) + elementLen;
				matchLenB = (int)(curPosB - innerB) + elementLen;
			}
			else
			{
				matchLenA = (int)(curPosA - lastPosA);
				matchLenB = (int)(curPosB - lastPosB);
			}
			if (matchLenB < matchLenA)
			{
				similarity += matchLenB * 100 / matchLenA;
			}
			else
			{
				similarity += matchLenA * 100 / matchLenB;
			}

			// ��¼�ϴ�ƥ�䵽��λ��
			lastPosA = curPosA;
			lastPosB = curPosB;
		}
		element += stepB;
	}

	similarity /= sectionsA + sectionsB;
	if (innerLenA == innerLenB)
	{
		similarity = (100 * sameLen + similarity * innerLenA) / lengthA;
	}
	else
	{
		similarity = ((100 * sameLen + similarity * innerLenA) / lengthA + (100 * sameLen + similarity * innerLenB) / lengthB) / 2;
	}
	return similarity;
}


//int main(int argc, char *argv[])
//{
//	int i;
//	i = get_web_page_similarity(argv[1], strlen(argv[1]), argv[2], strlen(argv[2]));
//	printf("%d\n", i);
//	return 0;
//}
