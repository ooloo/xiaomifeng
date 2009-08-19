#ifndef CRULEMGR_H_
#define CRULEMGR_H_

#include "global.h"
#include "CData.h"

class  CRule
{
public:
	CRule(void);
	~CRule(void);
	
	size_t process(const std::string& strRawData, CDataSets& dataSets, const std::string& strAddr, DATASET* pData);	///	原始数据处理函数，在处理的过程中直接产生分析出来的数据，提交给CDataSets
	
protected:
	size_t _genData(std::string& valRaw, CDataSets& dataSets,const RULESET* stRule,const std::string& strAddr);
	size_t _findPositiveDirection(const std::string& strRawData, CDataSets& dataSets,const RULESET& stRule,const std::string& strAddr);	///	正向查找
	size_t _findReverse(const std::string& strRawData, CDataSets& dataSets,const RULESET& stRule,const std::string& strAddr);	/// 反向查找
	void _netLabelFiltering(std::string& strRawData)const;
	void _dataSpecialCharacterFiltering(std::string& strRawData)const;
	void _setDataSet(CDataSets& dataSets);
	void _setDataSet(DATASET* pdataset);
public:
	//eDataType	_type;		///	规则值的类型：数据、地址、逻辑
	//INDEX		_layer;		///	网页层次id，从0层开始，0层表示起始抓取的页面
	//RULEINDEX	_identify;	///	规则在层内部的id
	//bool		_direction;	///	查找方向，true表示从begin开始查找，false表示从end开始查找
	//std::string _begin;
	//std::string _end;
	//std::string _name;	///	规则的名字
	//std::string _assistantdata; //辅助数据
	RULESET m_ruleset;
	RULESET* m_NextRuleSet;
	DATASET* m_pNewDataSet;
	DATASET* m_pNextDataSet;
	DATASET* m_pDataSetLink;
};

typedef std::list<CRule> RuleSets;

class CRuleMgr
{
public:
	CRuleMgr();
	virtual ~CRuleMgr();
	
	bool loadRuleFromToFile( const char* pszFileName );
	size_t insertRuleObj(const CRule& obj);
	
	const CRule* getRule(const INDEX iLayer, const RULEINDEX id)const;	///	得到某个规则
	size_t getRuleSets(const INDEX iLayer, RuleSets& sets)const;	/// 得到某一层的规则集合
	
private:
	RuleSets m_Rules;	///	所有规则集合
};

#endif /*CRULEMGR_H_*/
