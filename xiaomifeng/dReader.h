#ifndef DEXCELREADER_H_
#define DEXCELREADER_H_

#include "predefine.h"
#include <vector>

class dFileReader
{
	typedef std::vector< std::string > tStringArray1D;
	typedef std::vector< tStringArray1D > tStringArray2D;
	typedef enum
	{
		TXT_LINE,
		TXT_FIEL
	}READ_MODE;
public:
	//construct / destruct
		dFileReader()
			: m_pFile( NULL )
		{
		}
		virtual ~dFileReader()
		{
		}
		//���ļ������ļ���λ�ú��ļ�����
		bool readText( const std::string& strFile );
		bool readCSV( const std::string& strFile );
		bool readTextLinux(const std::string& strFile);
		const char* readContent();
		///������е�Ԫ������,2D����
		tStringArray2D& getElement()
		{
			return m_arrayElement;
		}
		bool writeText(const std::string& strAddr,const std::string& strContant);
		bool writeCSV(const std::string& fName,const std::string& strContant);
		bool spellCSV(const std::string& fName,const std::string& strContent);
		int GetRowN() {
			return m_nRow;
		}
		int GetColN() {
			return m_nRoll;
		}
		
		
	protected:
		//�ļ�ָ��
		FILE* m_pFile;
		tStringArray2D m_arrayElement;
		/// roll row
		int m_nRoll;//
		int m_nRow;//��������
};

#endif /*DEXCELREADER_H_*/
