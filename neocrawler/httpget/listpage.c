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

// 计算最小编辑距离，n和m表示s和t的长度
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

#define SIMILAR_LONG_SECTION		13		// 对较长的特征分为13段
#define SIMILAR_SHORT_SECTION		11		// 对较短的特征分为11段
#define SIMILAR_SMALL_ELEMENT_LEN	10		// 比较短元素的长度
#define SIMILAR_MID_ELEMENT_LEN		40		// 比较中元素的长度
#define SIMILAR_BIG_ELEMENT_LEN		80		// 比较长元素的长度

// 计算两个网页的相似度
int get_web_page_similarity(const char *characterA, int lengthA, const char *characterB, int lengthB)
{
	const char *innerA, *innerB;		// 内部的A和B，定义A为memcmp较小的那个
	int innerLenA, innerLenB;
	int minLen, maxLen;					// A和B的最小长度和最大长度
	int sameLen;						// A,B开始和结尾处相同的长度
	const char *tailA, *tailB;
	int tailLen;						// 结尾处相同的长度
	int sectionsA, sectionsB;			// A,B的分段数
	int elementLen;						// 选定的比较元素的长度
	int stepA, stepB;					// A,B的每个分段的步进长度
	const char *element;				// 需要匹配的元素
	const char *lastPosA, *lastPosB;	// A,B上个匹配到元素的位置
	const char *curPosA, *curPosB;		// A,B正在匹配的元素的位置
	const char *searchBegin, *searchEnd;	// 需要查找element的目标串范围
	int matchLenA, matchLenB;			// A,B当前匹配到元素和上个匹配到元素之间的距离
	int i;
	int similarity;						// 相似度

	minLen = lengthA > lengthB ? lengthB : lengthA;
	maxLen = lengthA > lengthB ? lengthA : lengthB;
	if (minLen <= 0 || characterA == NULL || characterB == NULL)
		return -1;

	// 确定innerA和innerB，并确定开头相同的长度
	innerA = characterA;
	innerB = characterB;
	i = minLen;
	while (i > 0 && *innerA == *innerB )
	{
		innerA++;
		innerB++;
		i--;
	}
	// 开始相同的长度
	sameLen = minLen - i;

	// 确定innerA和innerB，以及它们的长度
	if (*innerA < *innerB)
	{
		innerLenA = lengthA - sameLen;
		innerLenB = lengthB - sameLen;
	}
	else if (*innerA > *innerB)
	{
		// 交换innerA和innerB
		innerA = innerB;
		innerLenA = lengthB - sameLen;
		innerB = characterA + sameLen;
		innerLenB = lengthA - sameLen;
	}
	else
	{
		// 整个较短的那个与较长那个的开始处完全相同
		return minLen * 100 / maxLen;
	}

	minLen = innerLenA > innerLenB ? innerLenB : innerLenA;
	maxLen = innerLenA > innerLenB ? innerLenA : innerLenB;

	// 确定尾部相同的长度
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
		// 整个较短的那个与较长那个的开头和结尾处完全相同
		return (minLen + sameLen) * 100 / (maxLen + sameLen);
	}

	// 结尾相同的长度
	tailLen = minLen - i;
	// sameLen为首尾相同总长度
	sameLen += tailLen;

	// 此后tail指向结束位置，即最后一个有效字符的下一个位置
	tailA++;
	tailB++;

	// 此后的A和B被掐头去尾
	innerLenA -= tailLen;
	innerLenB -= tailLen;
	minLen -= tailLen;
	maxLen -= tailLen;

	similarity = minLen * 100 / maxLen;
	if (similarity < 50)
	{
		// 剩余长度相差50%以上，不进行比较了
		similarity = (100 * sameLen + similarity * minLen) / (maxLen + sameLen);
		return similarity;
	}

	if (minLen < 4 * SIMILAR_SMALL_ELEMENT_LEN)
	{
		// 对于比较短的特征，使用编辑距离计算相似度
		similarity = maxLen - levenshtein_distance(innerA, innerLenA, innerB, innerLenB);
		similarity = 100 * (sameLen + similarity) / (maxLen + sameLen);
		return similarity;
	}

	// 确定sectionsA和sectionB
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

	// 确定使用元素大小
	if (minLen > SIMILAR_LONG_SECTION * SIMILAR_BIG_ELEMENT_LEN * 8)
		elementLen = SIMILAR_BIG_ELEMENT_LEN;
	else if (minLen > SIMILAR_LONG_SECTION * SIMILAR_MID_ELEMENT_LEN * 8)
		elementLen = SIMILAR_MID_ELEMENT_LEN;
	else
		elementLen = SIMILAR_SMALL_ELEMENT_LEN;

	similarity = 0;

	// 对A进行分段
	stepA = (innerLenA - elementLen) / (sectionsA - 1);
	element = innerA;
	lastPosA = NULL;
	lastPosB = NULL;
	for (i = 0; i < sectionsA; i++)
	{
		curPosA = element;
		// 计算查找开始和结束位置
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

		// 查找element
		curPosB = memmem(searchBegin, searchEnd, element, elementLen);
		if (curPosB)
		{
			// 找到
			if (lastPosA == NULL)
			{
				// 匹配第一个元素时，需要加上elementLen
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

			// 记录上次匹配到的位置
			lastPosA = curPosA;
			lastPosB = curPosB;
		}
		element += stepA;
	}

	// 对B进行分段
	stepB = (innerLenB - elementLen) / (sectionsB - 1);
	element = innerB;
	lastPosA = NULL;
	lastPosB = NULL;
	for (i = 0; i < sectionsB; i++)
	{
		curPosB = element;
		// 计算查找开始和结束位置
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

		// 查找element
		curPosA = memmem(searchBegin, searchEnd, element, elementLen);
		if (curPosA)
		{
			// 找到
			if (lastPosB == NULL)
			{
				// 匹配第一个元素时，需要加上elementLen
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

			// 记录上次匹配到的位置
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
