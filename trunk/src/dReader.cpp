//#include "stdafx.h"
#include "dReader.h"

bool dFileReader::readText(const std::string& strFile )
{
	m_pFile = fopen( strFile.c_str(), "r+" );
	if( m_pFile == NULL )
	{
		return false;
	}

    //读文�?
	fseek( m_pFile, 0, SEEK_END );
	int nSize = ftell( m_pFile );
	fseek( m_pFile, 0, SEEK_SET );
     
    //���ļ��u�һ���ַ�������?
	char* szBuf = new char[nSize];
	memset( szBuf, 0, nSize );	

	fread( szBuf, 1, nSize, m_pFile );
	
	fclose( m_pFile );


	std::string strTemp;
	char szIter;
	int nStart = 0;
	int nRow = 0;
	tStringArray1D arrayStr;
	m_arrayElement.push_back( arrayStr );
    //������ļ����б��ｿ�ҵ��ո��س��������ӽ��?
	for( int i = 0; i < nSize; ++i )
	{
		//�ҵ��ļ��е�һ�����?
		szIter = *( szBuf + i );
		//����Ƿ�Ϊ�ո�?
		if( szIter == 9 )
		{
			//���ո���ǰ���ļ�����ƴ��
			for( int j = 0; j < i - nStart; j++ )
			{
				strTemp.append( szBuf + nStart + j, 1 );
			}
            //
			m_arrayElement[nRow].push_back( strTemp );
			
			strTemp.clear();
            m_nRoll += 1;
			nStart = i + 1;
		}
		//����Ƿ�Ϊ�س�?
		else if( szIter == 10 )
		{
			//���س���ǰ���ļ�����ƴ��
			for( int j = 0; j < i - nStart; j++ )
			{
				strTemp.append( szBuf + nStart + j, 1 );
			}

			m_arrayElement[nRow].push_back( strTemp );
			strTemp.clear();

			nStart = i + 1;

			nRow++;
			tStringArray1D arrayStr;
			m_arrayElement.push_back( arrayStr );
			m_nRoll = 0;
		}
		else
		{

		}
	}
	m_nRow = nRow;
	return true;
}
bool dFileReader::readCSV( const std::string& strFile )
{
	m_pFile = fopen( strFile.c_str(), "r+" );
	if( m_pFile == NULL )
	{
		return false;
	}

	//��ȡ�ļ�
	fseek( m_pFile, 0, SEEK_END );
	int nSize = ftell( m_pFile );
	fseek( m_pFile, 0, SEEK_SET );

	//���ļ��u�һ���ַ�������?
	char* szBuf = new char[nSize];
	memset( szBuf, 0, nSize );	

	fread( szBuf, 1, nSize, m_pFile );

	fclose( m_pFile );

	std::string strTemp;
	char szIter;
	int nStart = 0;
	int nRow = 0;
	tStringArray1D arrayStr;
	m_arrayElement.push_back( arrayStr );
	//������ļ����б��ｿ�ҵ��ո��س��������ӽ��?
	for( int i = 0; i < nSize; ++i )
	{
		//�ҵ��ļ��е�һ�����?
		szIter = *( szBuf + i );
		//����Ƿ�Ϊ����?
		if( szIter == 44 )
		{
			//���ո���ǰ���ļ�����ƴ��
			for( int j = 0; j < i - nStart; j++ )
			{
				strTemp.append( szBuf + nStart + j, 1 );
			}
			//
			m_arrayElement[nRow].push_back( strTemp );
			strTemp.clear();
			m_nRoll += 1;
			nStart = i + 1;
		}
		//����Ƿ�Ϊ�س�?
		else if( szIter == 10 )
		{
			
			//���س���ǰ���ļ�����ƴ��
			for( int j = 0; j < i - nStart; j++ )
			{
				strTemp.append( szBuf + nStart + j, 1 );
			}

			m_arrayElement[nRow].push_back( strTemp );
			m_arrayElement[nRow].push_back( "FULL" );
			strTemp.clear();

			nStart = i + 1;
            
			nRow++;
			tStringArray1D arrayStr;
			m_arrayElement.push_back( arrayStr );
			m_nRoll = 0;
		    }
		    else
		    {

		    }
	}
	m_nRow = nRow;
	//m_arrayElement[nRow].push_back( "NULL" );
	return true;
}

bool dFileReader::writeText(const std::string& strAddr,
							 const std::string& strContant)
{
	return true;
}
bool dFileReader::writeCSV(const std::string& fName,
							const std::string& strContant)
{
	m_pFile = fopen( fName.c_str(),"r+");
	if (m_pFile == NULL)
	{
		return false;
	}

	int nSize = (int)strContant.length();
	char* buffer = new char[nSize];
	memcpy(buffer,strContant.c_str(),nSize);
	fseek( m_pFile, 0, SEEK_END );
	size_t state = fwrite(buffer,sizeof(char),nSize,m_pFile);
	char szIte = 10;
	state = fwrite(&szIte,sizeof(char),1,m_pFile);
    fclose(m_pFile);
	return true;
}

bool dFileReader::readTextLinux(const std::string& strFile)
{
	return true;
}

bool dFileReader::spellCSV(const std::string& fName,
							const std::string& strContent)
{
	m_pFile = fopen( fName.c_str(),"r+");
	if (m_pFile == NULL)
	{
		m_pFile = fopen( fName.c_str(), "w+" );
		if ( m_pFile==NULL )
		{
			return false;
		}
		int size = (int)strContent.length();
		char* buffer = new char[size];
		memcpy(buffer,strContent.c_str(),size);
		fseek( m_pFile, 0, SEEK_END );
		
		size_t state = fwrite(buffer,sizeof(char),size,m_pFile);
		char szIte = 10;
		state = fwrite(&szIte,sizeof(char),1,m_pFile);
		fclose(m_pFile);
		return true;
	}
	else
	{
		return false;
	}
}
