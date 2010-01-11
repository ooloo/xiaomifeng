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
		//打开文件标明文件的位置和文件名称
		bool readText( const std::string& strFile );
		bool readCSV( const std::string& strFile );
		bool readTextLinux(const std::string& strFile);
		const char* readContent();
		///获得其中的元素数据,2D数据
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
		//文件指针
		FILE* m_pFile;
		tStringArray2D m_arrayElement;
		/// roll row
		int m_nRoll;//
		int m_nRow;//行向描述
};

#endif /*DEXCELREADER_H_*/
