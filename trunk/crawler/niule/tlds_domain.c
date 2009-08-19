#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <openssl/md5.h>

char domain_suffix[][32] = {
"AC",
"AD",
"AE",
"AERO",
"AF",
"AG",
"AI",
"AL",
"AM",
"AN",
"AO",
"AQ",
"AR",
"ARPA",
"AS",
"ASIA",
"AT",
"AU",
"AW",
"AX",
"AZ",
"BA",
"BB",
"BD",
"BE",
"BF",
"BG",
"BH",
"BI",
"BIZ",
"BJ",
"BM",
"BN",
"BO",
"BR",
"BS",
"BT",
"BV",
"BW",
"BY",
"BZ",
"CA",
"CAT",
"CC",
"CD",
"CF",
"CG",
"CH",
"CI",
"CK",
"CL",
"CM",
"CN",
"CO",
"COM",
"COOP",
"CR",
"CU",
"CV",
"CX",
"CY",
"CZ",
"DE",
"DJ",
"DK",
"DM",
"DO",
"DZ",
"EC",
"EDU",
"EE",
"EG",
"ER",
"ES",
"ET",
"EU",
"FI",
"FJ",
"FK",
"FM",
"FO",
"FR",
"GA",
"GB",
"GD",
"GE",
"GF",
"GG",
"GH",
"GI",
"GL",
"GM",
"GN",
"GOV",
"GP",
"GQ",
"GR",
"GS",
"GT",
"GU",
"GW",
"GY",
"HK",
"HM",
"HN",
"HR",
"HT",
"HU",
"ID",
"IE",
"IL",
"IM",
"IN",
"INFO",
"INT",
"IO",
"IQ",
"IR",
"IS",
"IT",
"JE",
"JM",
"JO",
"JOBS",
"JP",
"KE",
"KG",
"KH",
"KI",
"KM",
"KN",
"KP",
"KR",
"KW",
"KY",
"KZ",
"LA",
"LB",
"LC",
"LI",
"LK",
"LR",
"LS",
"LT",
"LU",
"LV",
"LY",
"MA",
"MC",
"MD",
"ME",
"MG",
"MH",
"MIL",
"MK",
"ML",
"MM",
"MN",
"MO",
"MOBI",
"MP",
"MQ",
"MR",
"MS",
"MT",
"MU",
"MUSEUM",
"MV",
"MW",
"MX",
"MY",
"MZ",
"NA",
"NAME",
"NC",
"NE",
"NET",
"NF",
"NG",
"NI",
"NL",
"NO",
"NP",
"NR",
"NU",
"NZ",
"OM",
"ORG",
"PA",
"PE",
"PF",
"PG",
"PH",
"PK",
"PL",
"PM",
"PN",
"PR",
"PRO",
"PS",
"PT",
"PW",
"PY",
"QA",
"RE",
"RO",
"RS",
"RU",
"RW",
"SA",
"SB",
"SC",
"SD",
"SE",
"SG",
"SH",
"SI",
"SJ",
"SK",
"SL",
"SM",
"SN",
"SO",
"SR",
"ST",
"SU",
"SV",
"SY",
"SZ",
"TC",
"TD",
"TEL",
"TF",
"TG",
"TH",
"TJ",
"TK",
"TL",
"TM",
"TN",
"TO",
"TP",
"TR",
"TRAVEL",
"TT",
"TV",
"TW",
"TZ",
"UA",
"UG",
"UK",
"UM",
"US",
"UY",
"UZ",
"VA",
"VC",
"VE",
"VG",
"VI",
"VN",
"VU",
"WF",
"WS",
"XN--0ZWM56D",
"XN--11B5BS3A9AJ6G",
"XN--80AKHBYKNJ4F",
"XN--9T4B11YI5A",
"XN--DEBA0AD",
"XN--G6W251D",
"XN--HGBK6AJ7F53BBA",
"XN--HLCJ6AYA9ESC7A",
"XN--JXALPDLP",
"XN--KGBECHTV",
"XN--ZCKZAH",
"YE",
"YT",
"YU",
"ZA",
"ZM",
"ZW",
};

int suffixNum = 0;
pthread_mutex_t suffix_mutex=PTHREAD_MUTEX_INITIALIZER;

static int compmi(const void *m1, const void *m2)
{
	return strcasecmp((char*)m1, (char*)m2);
}

int get_domainnames(char *domain, char *domainnames) 
{
	int pos=0,flag=0;
	char *res, *token;
	char tmpdomain[128];

	if(suffixNum == 0)
	{
		suffixNum = sizeof(domain_suffix)/sizeof(domain_suffix[0]);
		qsort(domain_suffix, suffixNum, sizeof(domain_suffix[0]), compmi);
	}
	
	snprintf(tmpdomain, 128, "%s", domain);
	token = strtok(tmpdomain, ".");
	while(token != NULL)
	{
		//printf("%s\n", token);
		res = bsearch(token, domain_suffix, suffixNum, sizeof(domain_suffix[0]), compmi);
		if (res == NULL)
		{
			flag = 0;
			pos = token - tmpdomain;
		}
		else
		{
			flag = 1;
		}
		token = strtok(NULL, "."); 
	}

	if(flag > 0)
	{
		strcpy(domainnames, domain + pos);
		//printf("%s\n", domainnames);
		return 0;
	}
	
	return -1;
}

int insite_check(char *srcUrl, char *linkUrl)
{
	int ret,len;
	char *p,*t;
	char domain[128],dm1[128], dm2[128];

	t = strstr(srcUrl, "://");
	if(t == NULL)
		return -1;
	else
		t += 3;

	p = t;
	while(*p)
	{
		if(*p == ':' || *p == '/')
			break;
		p++;
	}

	len = p - t;
	if(len >= 128)
		return -1;

	memcpy(domain, t, len);
	*(domain + len) = 0;
	
	pthread_mutex_lock(&suffix_mutex);
	ret = get_domainnames(domain, dm1);
	pthread_mutex_unlock(&suffix_mutex);
	if(ret == -1)
		return -1;

	t = strstr(linkUrl, "://");
	if(t == NULL)
		return -1;
	else
		t += 3;

	p = t;
	while(*p)
	{
		if(*p == ':' || *p == '/')
			break;
		p++;
	}

	len = p - t;
	if(len >= 128)
		return -1;

	memcpy(domain, t, len);
	*(domain + len) = 0;
	
	pthread_mutex_lock(&suffix_mutex);
	ret = get_domainnames(domain, dm2);
	pthread_mutex_unlock(&suffix_mutex);
	if(ret == -1)
		return -1;

	if(strcasecmp(dm1, dm2) != 0)
		return 0;
	else
		return 1;
}

#ifdef DEBUG
int main(int argc, char **argv)
{
	int ret;
	char domainnames[64];
	get_domainnames(argv[1], domainnames);

	ret = insite_check("http://df.fddjaf.com.de/aaa", "http://a.fdjaf.com.cn/dasio.fd.fdj");
	printf("%d\n", ret);

	return 0;
}
#endif
