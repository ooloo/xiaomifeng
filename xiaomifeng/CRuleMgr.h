#ifndef CRULEMGR_H_
#define CRULEMGR_H_

#include "global.h"
#include "CData.h"

class  CRule
{
public:
	CRule(void);
	~CRule(void);
	
	size_t process(const std::string& strRawData, CDataSets& dataSets, const std::string& strAddr, DATASET* pData);	///	ԭʼ���ݴ��������ڴ���Ĺ�����ֱ�Ӳ����������������ݣ��ύ��CDataSets
	
protected:
	size_t _genData(std::string& valRaw, CDataSets& dataSets,const RULESET* stRule,const std::string& strAddr);
	size_t _findPositiveDirection(const std::string& strRawData, CDataSets& dataSets,const RULESET& stRule,const std::string& strAddr);	///	�������
	size_t _findReverse(const std::string& strRawData, CDataSets& dataSets,const RULESET& stRule,const std::string& strAddr);	/// �������
	void _netLabelFiltering(std::string& strRawData)const;
	void _dataSpecialCharacterFiltering(std::string& strRawData)const;
	void _setDataSet(CDataSets& dataSets);
	void _setDataSet(DATASET* pdataset);
public:
	//eDataType	_type;		///	����ֵ�����ͣ����ݡ���ַ���߼�
	//INDEX		_layer;		///	��ҳ���id����0�㿪ʼ��0���ʾ��ʼץȡ��ҳ��
	//RULEINDEX	_identify;	///	�����ڲ��ڲ���id
	//bool		_direction;	///	���ҷ���true��ʾ��begin��ʼ���ң�false��ʾ��end��ʼ����
	//std::string _begin;
	//std::string _end;
	//std::string _name;	///	���������
	//std::string _assistantdata; //��������
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
	
	const CRule* getRule(const INDEX iLayer, const RULEINDEX id)const;	///	�õ�ĳ������
	size_t getRuleSets(const INDEX iLayer, RuleSets& sets)const;	/// �õ�ĳһ��Ĺ��򼯺�
	
private:
	RuleSets m_Rules;	///	���й��򼯺�
};

#endif /*CRULEMGR_H_*/
