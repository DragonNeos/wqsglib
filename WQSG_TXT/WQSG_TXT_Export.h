/*  WQSG_Lib
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
#ifndef __WQSG_TXT_Export_H__
#define __WQSG_TXT_Export_H__

#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#include<WQSG_DEF.h>
#include<WQSG.h>
#include<WQSG_File_M.h>

#include "MapTbl_Base.h"
/***************************************************************************
指针格式(共)		DEC
指针个数(共)		HEX
指针表首项地址(共)		HEX
文本基础地址(共)		HEX
文件头长(共)		HEX
表项长度(共)		DEC
指针实际长度(共)		DEC
指针倍率(共)		DEC
指针表结束地址(带长度)	DEC
***************************************************************************/
#define WQSG_TXT_BUF 10485760

namespace WQSG_TEXT_OUT
{

}
//============================================================================
class CWQSG_MapTbl_OUT :public CWQSG_MapTbl_Base
{
	BOOL m_b禁止码表错误;
	inline BOOL Add_TBL( CMemTextW& a_tp , const WCHAR*const a_pPathName );
	inline BOOL Add_TBL2( CMemTextW& a_tp , const WCHAR*const a_pPathName , const BOOL 不需要结束符 );
	inline u8* Hex2Bin( const WCHAR* a_pText , INT a_iSYLen )
	{
		u8*const pSY = new UCHAR[a_iSYLen];
		u8* s1 = pSY;

		for( INT val ; a_iSYLen > 0 ; --a_iSYLen )
		{
			::swscanf_s( a_pText++ , L"%02X" , &val );
			*s1 = (u8)val;
			++s1;
			a_pText++;
		}

		return pSY;
	}
protected:
	BOOL m_bCheckTbl;
	INT m_MAX_Byte;

	CWD_tree_del<WCHAR> m_Text;
/////////////////////////////////////////////////////////////
public:
	inline BOOL LOAD_TBL( WCHAR const*const a_pPathName , BOOL a_bCheckTbl );
	inline BOOL LOAD_TBL2( WCHAR const*const TBL2_path , BOOL a_bCheckTbl , const BOOL 不需要结束符 = TRUE );
	inline CWQSG_MapTbl_OUT( );
	virtual	inline ~CWQSG_MapTbl_OUT();
	inline void ClearTbl(void);

	const u64 m_OVER , m_TURN , m_END;

	inline INT GetMaxByte()const
	{
		return m_MAX_Byte;
	}

	inline const CWD_tree_del<WCHAR>& GetText()const
	{
		return m_Text;
	}
};

inline BOOL CWQSG_MapTbl_OUT::Add_TBL( CMemTextW& a_tp , const WCHAR*const a_pPathName )
{
	while( WCHAR* WTXT_ALL = a_tp.GetLine() )
	{
		///判断空行 WTXT_ALL
		WCHAR* pTempText = ::WQSG_DelSP_ALL( WTXT_ALL );
		if( !(*pTempText) || *(u16*)pTempText == *(u16*)L"//" )
		{
			delete[]WTXT_ALL;
			delete[]pTempText;
			continue;
		}
		delete[]pTempText;

		INT nItmp , nJtmp;
		// WTXT_ALL
		nItmp = ::WQSG_strchr( WTXT_ALL , L'=' );
		if( -1 == nItmp )//错误的码表格式,找不到"="
		{
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n错误的码表格式(=)" , a_pPathName );

			if( m_b禁止码表错误 )
			{
				delete WTXT_ALL;
				return FALSE;
			}

			delete[]WTXT_ALL;
			continue;
		}
		WCHAR* WTXT_L = ::WQSG_getstrL( WTXT_ALL , nItmp );//取等号左边
		pTempText = WTXT_L ;
		WTXT_L = ::WQSG_DelSP_ALL( pTempText );
		delete[]pTempText;
		nJtmp = ::WQSG_strlen(WTXT_L);//验证长度
		if(!::WQSG_是十六进制文本(WTXT_L) //不是HEX
			|| (nJtmp&1)//单数
			|| !(nJtmp>>1)//为0
			)
		{
			delete[]WTXT_L;

			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n码表左边必须是(HEX)" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}

			delete[]WTXT_ALL;
			continue;
		}
		//取右边delete WTXT_L;delete WTXT_ALL;

		WCHAR*WTXT_R = ::WQSG_getstrL (WTXT_ALL + nItmp + 1,-1);//取等号右边
		nItmp = ::WQSG_strlen(WTXT_R);
		if( nItmp <= 0 )//右边不是一个字,或者没有
		{
			delete[]WTXT_L;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n码表右边必须是一个字" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}
		nItmp = nJtmp>>1;
		////////////////////////////////////////////////////////////////////////////////// 加入树

		u8*const pSY = Hex2Bin( WTXT_L , nItmp );
		delete[]WTXT_L;

		pTempText = m_Text.索引( pSY , nItmp );
		//验证重复
		if(pTempText)
		{
			if(m_bCheckTbl)
			{
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n码表重复" , a_pPathName );
				if( m_b禁止码表错误 )
				{
					delete[]WTXT_ALL;delete[]WTXT_R;delete[]pSY;
					return FALSE;
				}
			}
			delete pTempText;
		}
		delete[]WTXT_ALL;

		//添加
		m_Text.压入( pSY , nItmp , WTXT_R );
		delete[]pSY;

		if( nItmp > m_MAX_Byte )
			m_MAX_Byte = nItmp;
		/////////////////////////////////////////
	}
	return TRUE;
}

inline BOOL CWQSG_MapTbl_OUT::Add_TBL2( CMemTextW& a_tp , const WCHAR*const a_pPathName , const BOOL 不需要结束符 )
{
	BOOL 有结束符 = FALSE;
	while( WCHAR* WTXT_ALL = a_tp.GetLine() )
	{
		///判断空行 WTXT_ALL
		WCHAR* pTempText = ::WQSG_DelSP_ALL(WTXT_ALL);
		if( !(*pTempText) || *(u16*)pTempText == *(u16*)L"//" )
		{
			delete[]WTXT_ALL;
			delete[]pTempText;
			continue;
		}
		delete[]pTempText;

		INT nItmp , nJtmp;
		// WTXT_ALL

		nItmp = ::WQSG_strchr(WTXT_ALL,L'=');
		if( -1 == nItmp )//错误的码表格式,找不到"="
		{
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n错误的控制码表格式(=)" , a_pPathName );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}
		WCHAR* WTXT_L = ::WQSG_getstrL ( WTXT_ALL , nItmp );//取等号左边
		pTempText = WTXT_L;
		WTXT_L = ::WQSG_DelSP_ALL(pTempText);
		delete[]pTempText;
		nJtmp = ::WQSG_strlen(WTXT_L);//验证长度
		if(!::WQSG_是十六进制文本(WTXT_L) //不是HEX
			|| (nJtmp & 1)//单数
			|| !(nJtmp >> 1)//为0
			)
		{
			delete[]WTXT_L;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n控制码表左边必须是(HEX)" , a_pPathName );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}


		const INT iSY = nJtmp >> 1;
		u8*const pSY = Hex2Bin( WTXT_L , iSY );
		delete[]WTXT_L;

		//取右边delete pSY;delete WTXT_ALL;
		WCHAR* WTXT_R = ::WQSG_getstrL(WTXT_ALL + nItmp + 1,-1);//取等号右边
		nItmp = (int)::wcslen(WTXT_R);
		if( nItmp <= 0 )//无字
		{
			delete[]pSY;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n控制码表右边必须至少有一个字" , a_pPathName );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}
		if( nItmp > 255)
		{
			delete[]pSY;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n控制码表右边不能超过255个字" );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}
		///////////////////判断控制符
		pTempText = ::WQSG_abc_ABC(WTXT_R);
		if((*((u64*)pTempText) == m_OVER))
		{
			有结束符= TRUE;
			delete[]WTXT_R;
			WTXT_R = pTempText;

			pTempText += 4;

			if(*pTempText)
			{
				if(!::WQSG_是十进制文本(pTempText)
					||::wcslen(pTempText) > 3)
				{
					delete[]pSY;delete[]WTXT_R;
					CWQSG_strW errStr( WTXT_ALL );

					zzz_Log( errStr + L"\r\n制码表结束符数字错误\r\n    格式为: 编码=OVERx\r\n  OVER大小写都行\r\n    x 只能为十进制,不能有多于字符,最长位" , a_pPathName );

					delete[]WTXT_ALL;
					if( m_b禁止码表错误 )
						return FALSE;

					continue;
				}
			}
		}
		else if(*((u64*)pTempText) == m_TURN)
		{
			delete[]WTXT_R;
			WTXT_R = pTempText;

			pTempText += 4;

			if(!::WQSG_是十进制文本(pTempText)
				||::wcslen(pTempText) > 2)
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n控制码表跳跃符数字错误\r\n    格式为: 编码=TURNx\r\n  TURN大小写都行\r\n    x 只能为十进制,不能有多于字符,最长2位" , a_pPathName );

				delete[]WTXT_ALL;
				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}
		}
		else if( *((u64*)pTempText) == m_END )
		{
			delete[]WTXT_R;

			if( ::wcslen(pTempText) != 4 )
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n\"END:\"后面不能有多余的字符" , a_pPathName );

				delete[]WTXT_ALL;

				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}

			WCHAR* pTxt = m_Text.索引( pSY , iSY );
			if( NULL == pTxt )
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n与当前\"END:\"对应编码的项不能为空" , a_pPathName );

				delete[]WTXT_ALL;

				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}

			if( 1 != ::WQSG_strlen( pTxt ) )
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n与当前\"END:\"对应编码的项必须为一个字符" , a_pPathName );

				delete[]WTXT_ALL;

				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}

			WTXT_R = new WCHAR[6];

			WQSG_strcpy( pTempText , WTXT_R );
			WTXT_R[4] = *pTxt;
			WTXT_R[5] = 0;

			delete[]pTempText;
			delete[]pTxt;

			m_Text.压入( pSY , iSY , NULL );
		}
		else
		{
			delete[]pTempText;
			pTempText = WTXT_R;

			WTXT_R = new WCHAR[nItmp + 3];
			WCHAR *Wtmp2 = WTXT_R;
			*(Wtmp2++) = L'{';
			Wtmp2 += ::WQSG_strcpy(pTempText,Wtmp2);
			*(Wtmp2++) = L'}';
			*Wtmp2 = 0;
			delete[]pTempText;
		}
		////////////////////////////////////////////////////////////////////////////////// 加入树

		pTempText = m_Text.索引( pSY , iSY );
		////////////////////
		if(pTempText)
		{
			if(m_bCheckTbl)
			{
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n控制码表重复" , a_pPathName );

				if( m_b禁止码表错误 )
				{
					delete[]WTXT_ALL;delete[]WTXT_R;delete[]pSY;
					return FALSE;
				}
			}
			delete pTempText;
		}
		delete[]WTXT_ALL;
		/////////////////////////////////////////

		m_Text.压入( pSY , iSY , WTXT_R );
		delete[]pSY;

		if( iSY > m_MAX_Byte )
			m_MAX_Byte = iSY;
	}
	if(不需要结束符 || 有结束符)
		return TRUE;

	zzz_Log( L"\r\n控制符至少需要一个结束符" , a_pPathName );
	return FALSE;		
}

inline BOOL CWQSG_MapTbl_OUT::LOAD_TBL( WCHAR const*const a_pPathName , BOOL a_bCheckTbl )
{
	m_bCheckTbl = a_bCheckTbl;

	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		//zzz_Log();
		return FALSE;
	}

	return Add_TBL( tp , a_pPathName );
}

inline BOOL CWQSG_MapTbl_OUT::LOAD_TBL2( WCHAR const*const a_pPathName , BOOL a_bCheckTbl , const BOOL 不需要结束符 )
{
	m_bCheckTbl = a_bCheckTbl;

	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		//zzz_Log();
		return FALSE;
	}

	return Add_TBL2( tp , a_pPathName , 不需要结束符 );
}

inline CWQSG_MapTbl_OUT::CWQSG_MapTbl_OUT( )
: CWQSG_MapTbl_Base( )
, m_b禁止码表错误(TRUE)
, m_OVER( *((u64*)L"OVER") )
, m_TURN( *((u64*)L"TURN") )
, m_END( *((u64*)L"END:") )
{
}

inline CWQSG_MapTbl_OUT::~CWQSG_MapTbl_OUT()
{
}

inline void CWQSG_MapTbl_OUT::ClearTbl(void)
{
	m_MAX_Byte = 0;
	m_Text.清空();
}
//----------------------------------------------------------------------------
class CWQSG_TXT_O
{
	inline void zzz_写出文本( CWQSG_File& a_fp , s64 a_nAddr , UINT a_uLen , WCHAR*const a_pTextEnd , const WCHAR*const a_pTXT_path )
	{
		if( a_uLen > m_MIN_OUT && a_uLen < m_MAX_OUT )
		{
			::WQSG_strcpy ( L"\r\n\r\n" , a_pTextEnd );

			WCHAR addr_len[20];
			::swprintf( addr_len , L"%08X,%u," , (u32)( a_nAddr & 0xFFFFFFFF ) , a_uLen );

			if( !a_fp.WriteStrW( addr_len ) )
			{
				zzz_Log( L"写文本失败"  , a_pTXT_path );
			}

			if( !a_fp.WriteStrW( m_pBUF_OUT ) )
			{
				zzz_Log( L"写文本失败"  , a_pTXT_path );
			}
		}
	}
#ifndef _____________________________________________
	inline BOOL 正式导出( CWQSG_File& a_fp , WQSG_FM_FILE& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path );
#else
	BOOL 正式导出( CWQSG_File& a_fp , WQSG_File_mem& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path );
#endif
private:
	UINT m_MAX_OUT;
	UINT m_MIN_OUT;
	WCHAR* m_pBUF_OUT;
	CWQSG_MapTbl_OUT m_Tbl;

	inline static void zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle = NULL )
	{
		CWQSG_MapTbl_OUT::zzz_Log( a_szText , a_szTitle );
	}
public:
	inline CWQSG_TXT_O( );
	virtual	inline ~CWQSG_TXT_O();
	inline void ClearTbl(void);
	inline BOOL LoadTbl( const WCHAR* a_pPathName , const WCHAR* TBL2_path , BOOL a_bCheckTbl = FALSE );
#ifndef _____________________________________________
	inline BOOL 导出文本( const WCHAR* rom_path , const WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 );
#else
	BOOL 导出文本( WCHAR* rom_path , WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 );
#endif
};


#ifndef _____________________________________________
BOOL CWQSG_TXT_O::正式导出( CWQSG_File& a_fp , WQSG_FM_FILE& mFile_ROM , s64 开始位置 , s64 剩余长度 , const WCHAR*const TXT_path )
{
	s64		ot_addr = 开始位置;
	UINT	ot_len = 0;
	WCHAR*	ot_txt = m_pBUF_OUT;

	*ot_txt = L'\0';

	u8 tmp_C[1024];

	const CWD_tree_del<WCHAR>& text = m_Tbl.GetText();
	//--------------------------------------
	while( 剩余长度 )
	{
		UINT BufferLen = mFile_ROM.GetUCHAR( ( 剩余长度 > m_Tbl.GetMaxByte() )?m_Tbl.GetMaxByte():(UINT)剩余长度 , tmp_C );

		if( 0 == BufferLen )
			break;

		UINT 索引长度;
		WCHAR* tmp2 = text.匹配( tmp_C , BufferLen , 索引长度 );

		if( tmp2 )
		{
			剩余长度 -= 索引长度;
			开始位置 += 索引长度;
			ot_len += 索引长度;
			mFile_ROM.BACK( BufferLen - 索引长度 );

			if( *((u64*)tmp2) == m_Tbl.m_OVER )
			{//是结束符
				ot_len -= 索引长度;

				if( tmp2[4] )
				{
					::swscanf_s( tmp2 + 4 , L"%u" , &BufferLen );
					if( BufferLen > 剩余长度 )
						剩余长度 = 0;
					else
						剩余长度 -= BufferLen;

					开始位置 += BufferLen;
					mFile_ROM.SeekADD( BufferLen );
				}
				/*
				s1 += ::WQSG_strcpy(L"{结束符",s1);
				for(UINT i = 0;i < 索引长度;i++){
				::swprintf_s((s1++),WQSG_TXT_BUF,L"%02X",tmp_C[i]);
				s1++;
				}
				*(s1++) = L'}';*/
				/////////////////////////////
				zzz_写出文本( a_fp , ot_addr , ot_len , ot_txt , TXT_path );
				ot_txt = m_pBUF_OUT;
				*m_pBUF_OUT = 0;
				ot_len = 0;
				ot_addr = 开始位置;
			}
			else if(*((u64*)tmp2) == m_Tbl.m_TURN)
			{//是跳跃符
				ot_txt += ::WQSG_strcpy( L"{跳跃符" , ot_txt );
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , tmp_C[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'：';
				///////////////
				::swscanf( tmp2 + 4 , L"%u" , &BufferLen );

				索引长度 = mFile_ROM.GetUCHAR( BufferLen , tmp_C );

				if( 索引长度 > 剩余长度 )
					剩余长度 = 0;
				else
					剩余长度 -= 索引长度;

				开始位置 += 索引长度;
				ot_len += 索引长度;
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , tmp_C[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'}';
			}
			else if(*((u64*)tmp2) == m_Tbl.m_END)
			{
				ot_txt += ::WQSG_strcpy( tmp2 + 4 , ot_txt );

				/////////////////////////////
				zzz_写出文本( a_fp , ot_addr , ot_len , ot_txt , TXT_path );
				ot_txt = m_pBUF_OUT;
				*m_pBUF_OUT = 0;
				ot_len = 0;
				ot_addr = 开始位置;
			}
			else
			{//通常
				ot_txt += ::WQSG_strcpy( tmp2 , ot_txt );
			}
		}
		else
		{
			mFile_ROM.BACK( --BufferLen );
			--剩余长度;
			++开始位置;
			zzz_写出文本( a_fp , ot_addr , ot_len , ot_txt , TXT_path );
			ot_txt = m_pBUF_OUT;
			*m_pBUF_OUT = 0;
			ot_len = 0;
			ot_addr = 开始位置;
		}
	}
	zzz_写出文本( a_fp , ot_addr , ot_len , ot_txt , TXT_path );

		return TRUE;
}
#else
BOOL CWQSG_TXT_O::正式导出( CWQSG_File& a_fp , WQSG_File_mem& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path )
{
	s64		ot_addr = 开始位置;
	UINT	ot_len = 0;
	WCHAR*	ot_txt = BUF_OUT;

	*ot_txt = L'\0';
	//--------------------------------------
	while( 剩余长度 )
	{
		const u32 BufferLen = (剩余长度 > MAX_字节 )?(MAX_字节):(u32)剩余长度;

		u8 const*const Buffer = mFile_ROM.GetPtr( 开始位置 , BufferLen );
		if( !Buffer )
		{
			MessageBoxW( L"未知错误1???" , TXT_path );
			return FALSE;
		}
		//-------------------------------------
		UINT 索引长度 = BufferLen;
		UINT	xlen = 0;
		WCHAR const* tmp2 = NULL;
		{
			节点_TXT_W**	tmpTree = m_ROOT;

			u8 const* SY_tmp = Buffer;
			while( 索引长度-- )
			{
				if( 节点_TXT_W*const tmpNode = tmpTree[*(SY_tmp++)] )
				{//存在下一层
					if( tmpNode->m_TXT_W )
					{
						tmp2 = tmpNode->m_TXT_W;
						xlen = 索引长度;
					}
					tmpTree = tmpNode->m_next;
				}
				else
				{//不存在下一层
					if( tmp2 )
					{
						索引长度 = --xlen;
					}
					break;
				}
			}
			索引长度 = BufferLen - (++索引长度);
		}
		//------------------------------------------------
		if( tmp2 )
		{
			剩余长度 -= 索引长度;
			开始位置 += 索引长度;

			ot_len += 索引长度;
			if( *((u64*)tmp2) == OVER )
			{//是结束符
				ot_len -= 索引长度;

				if( tmp2[4] )
				{
					::swscanf_s( tmp2 + 4 , L"%u" , &索引长度 );
					if( 索引长度 > 剩余长度 )
						剩余长度 = 0;
					else
						剩余长度 -= 索引长度;

					开始位置 += 索引长度;
				}
				/*
				s1 += ::WQSG_strcpy(L"{结束符",s1);
				for(UINT i = 0;i < 索引长度;i++){
				::swprintf_s((s1++),WQSG_TXT_BUF,L"%02X",tmp_C[i]);
				s1++;
				}
				*(s1++) = L'}';*/
				/////////////////////////////
				DEF_写出文本( ot_addr , ot_len , ot_txt )
					ot_txt = BUF_OUT;
				*BUF_OUT = 0;
				ot_len = 0;
				ot_addr = 开始位置;
			}
			else if(*((U64*)tmp2) == TURN)
			{//是跳跃符
				ot_txt += ::WQSG_strcpy( L"{跳跃符" , ot_txt );
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , Buffer[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'：';
				///////////////
				::swscanf( tmp2 + 4 , L"%u" , &索引长度 );
				if( 索引长度 > 剩余长度 )
				{
					索引长度 = (UINT)剩余长度;
					剩余长度 = 0;
				}
				else
					剩余长度 -= 索引长度;

				u8 const*const buf = mFile_ROM.GetPtr( 开始位置 , 索引长度 );
				if( NULL == buf )
				{
					MessageBoxW( L"未知错误2???" , TXT_path );
					mFile_ROM.Free();
					return FALSE;
				}

				开始位置 += 索引长度;

				ot_len += 索引长度;
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , buf[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'}';
			}
			else
			{//通常
				ot_txt += ::WQSG_strcpy( tmp2 , ot_txt );
			}
		}
		else
		{
			--剩余长度;
			++开始位置;
			DEF_写出文本( ot_addr , ot_len , ot_txt )
				ot_txt = BUF_OUT;
			*BUF_OUT = 0;
			ot_len = 0;
			ot_addr = 开始位置;
		}
		mFile_ROM.Free();
	}
	DEF_写出文本( ot_addr , ot_len , ot_txt )

		return TRUE;
}
#endif

CWQSG_TXT_O::CWQSG_TXT_O( )
{
	m_pBUF_OUT = new WCHAR[ WQSG_TXT_BUF + 1 ];
}

CWQSG_TXT_O::~CWQSG_TXT_O()
{
	delete[]m_pBUF_OUT;
}

void CWQSG_TXT_O::ClearTbl(void)
{
	m_Tbl.ClearTbl();
}

BOOL CWQSG_TXT_O::LoadTbl( const WCHAR* a_pPathName , const WCHAR* TBL2_path , BOOL a_bCheckTbl )
{
	ClearTbl();
	if( !m_Tbl.LOAD_TBL( a_pPathName , a_bCheckTbl ) )
	{
		ClearTbl();
		return FALSE;
	}
	if( TBL2_path )
	{
		if( *TBL2_path )
			if( !m_Tbl.LOAD_TBL2( TBL2_path , a_bCheckTbl ) )
			{
				ClearTbl();
				return FALSE;
			}
	}
	return TRUE;
}
#ifndef _____________________________________________
BOOL CWQSG_TXT_O::导出文本( const WCHAR* a_rom_path , const WCHAR* a_TXT_path , s64 a_KS , s64 a_JS , UINT a_MIN , UINT a_MAX )
{
	if( a_KS < 0 )
	{
		zzz_Log( L"开始地址 不能大于 0x7FFFFFFFFFFFFFFF"  , a_rom_path );
		return FALSE;
	}
	if( a_JS < 0 )
	{
		zzz_Log( L"结束地址 不能大于 0x7FFFFFFFFFFFFFFF"  , a_rom_path );
		return FALSE;
	}
	////////// 验证地址
	if( a_KS > a_JS )
	{
		zzz_Log( L"结束地址 不能小于 开始地址"  , a_rom_path );
		return FALSE;
	}
	*m_pBUF_OUT = 0;
	//////////////// 初始化变量
	m_MIN_OUT = a_MIN;
	m_MAX_OUT = a_MAX;
	///////////////////////////
	WQSG_FM_FILE mFile_ROM;
	if( !mFile_ROM.OpenFile( a_rom_path , 3 ) )
	{
		zzz_Log( L"打开ROM文件失败"  , a_rom_path );
		return FALSE;
	}
	////////// 验证地址
	const s64 romsize = mFile_ROM.Get文件长度();
	if( a_KS >= romsize )
	{
		zzz_Log( L"开始地址不存在,超出文件"  , a_rom_path );
		return FALSE;
	}
	if( a_JS >= romsize )
		a_JS = romsize - 1;
	/////////////////////
	CWQSG_File fp;
	if( !fp.OpenFile( a_TXT_path , 4 , 3 ) )
	{
		zzz_Log( L"创建文本文件失败"  , a_TXT_path );
		return FALSE;
	}
	if( !mFile_ROM.SeekTo(a_KS) )
	{
		zzz_Log( L"文件指针设置失败"  , a_rom_path );
		return FALSE;
	}
	if( 2 != fp.Write("\xFF\xFE" , 2 ) )
	{
		zzz_Log( L"写文本头失败"  , a_TXT_path );
		return FALSE;
	}
	if( a_JS > 0xFFFFFFFF )
	{
		u32 JJJ = (u32)(a_JS>>32);
		::swprintf_s( m_pBUF_OUT , 4096 , L".WQSG:%08X\r\n\r\n\r\n" , JJJ );
		if( fp.Write( m_pBUF_OUT , 40 ) )
		{
			zzz_Log( L"写文本信息头失败"  , a_TXT_path );
			return FALSE;
		}
	}
	return( 正式导出( fp , mFile_ROM , a_KS , (a_JS - a_KS + 1) , a_TXT_path ) );
}
#else
BOOL CWQSG_TXT_O::导出文本( WCHAR* rom_path , WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 )
{
	if( KS < 0 )
	{
		MessageBox( L"开始地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
		return FALSE;
	}
	if( JS < 0 )
	{
		MessageBox( L"结束地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
		return FALSE;
	}
	*BUF_OUT = 0;
	//////////////// 初始化变量
	MIN_OUT = MIN;
	MAX_OUT = MAX;
	///////////////////////////
	WQSG_File_mem mFile_ROM;
	if( !mFile_ROM.OpenFile( rom_path , 3 ) )
	{
		MessageBox( L"打开ROM文件失败"  , rom_path );
		return FALSE;
	}
	////////// 验证地址
	if( KS > JS )
	{
		MessageBox( L"结束地址 不能小于 开始地址"  , rom_path );
		return FALSE;
	}
	s64 romsize;
	mFile_ROM.GetFileSize( romsize );
	if( KS >= romsize )
	{
		MessageBox( L"开始地址不存在,超出文件"  , rom_path );
		return FALSE;
	}
	if( JS >= romsize )
		JS = romsize - 1;
	/////////////////////
	CWQSG_File fp;
	if( !fp.OpenFile( TXT_path , 4 , 3 ) )
	{
		MessageBox( L"创建文本文件失败"  , TXT_path );
		return FALSE;
	}
	//		if( !mFile_ROM.SeekTo(KS) )
	//		{
	//			MessageBox( L"文件指针设置失败"  , rom_path );
	//			return FALSE;
	//		}
	if( 2 != fp.Write("\377\376" , 2 ) )
	{
		MessageBox( L"写文本头失败"  , TXT_path );
		return FALSE;
	}
	if( JS > 0xFFFFFFFF )
	{
		u32 JJJ = (u32)(JS>>32);
		::swprintf_s( BUF_OUT , 4096 , L".WQSG:%08X\r\n\r\n\r\n" , JJJ );
		if( fp.Write( BUF_OUT , 40 ) )
		{
			MessageBox( L"写文本信息头失败"  , TXT_path );
			return FALSE;
		}
	}
	return( 正式导出( fp , mFile_ROM , KS , (JS - KS + 1) , TXT_path ) );
}
#endif
//----------------------------------------------------------------------------
class WQSG_pTXT_O:private CWQSG_MapTbl_OUT
{
	BOOL 读入指针表(WCHAR const*const ROM_path , WQSG_FM_FILE& mFile_ROM ,
		s64 首指针地址 ,u8 指针实长 ,u8 指针间隔 ,u8 指针倍率 ,u32 文本基础地址	)
	{
		s64 tmp64;
		CWQSG_TypeLinkList<s64> 链表数组;

		for(u32 i = 0;i < m_指针个数;i++)
		{
			if( !mFile_ROM.SeekTo( 首指针地址 ))
			{
				zzz_Log( L"未知错误1"  , ROM_path );
				return FALSE;
			}
			tmp64 = 0;
			if((UINT)指针实长 != mFile_ROM.GetUCHAR((UINT)指针实长,(const u8*)&tmp64))
			{
				zzz_Log( L"未知错误2"  , ROM_path );
				return FALSE;
			}
			tmp64 *= 指针倍率;
			tmp64 += 文本基础地址;

			if( 链表数组.AddItem(tmp64) < 0 )
			{
				zzz_Log( L"未知错误3"  , ROM_path );
				return FALSE;
			}

			首指针地址 += 指针间隔;
		}
		m_指向文本的地址 = 链表数组.MakeArray();
		if( NULL == m_指向文本的地址 )
		{
			zzz_Log( L"未知错误4"  , ROM_path );
			return FALSE;
		}
		return TRUE;
	}
	BOOL 地址_正式导出( WCHAR const*const ROM_path , CWQSG_File& a_fp , WQSG_FM_FILE& mFile_ROM )
	{
		u8 缓冲[1024];
		DWORD 长度;
		if( 2 != a_fp.Write("\xFF\xFE",2) )
		{
			zzz_Log( L"写出文本头失败" , ROM_path );
			return FALSE;
		}
		if( !a_fp.WriteStrW( BUF_OUT ) )
		{
				zzz_Log( L"写出信息头失败" , ROM_path );
			return FALSE;
		}
		///////////////////////////////////////////////////
		for( u32 指针号 = 0 ; 指针号 < m_指针个数 ; ++指针号 )
		{
			if( !mFile_ROM.SeekTo( m_指向文本的地址[指针号] ) )
			{
				::swprintf_s( BUF_OUT , WQSG_TXT_BUF,L"错误的指针地址\r\n%05u号,指向%08X位置,",指针号,m_指向文本的地址[指针号]);
				zzz_Log( BUF_OUT , ROM_path );
				return FALSE;
			}
			WCHAR* s1 = BUF_OUT;
			*BUF_OUT = 0;
			长度= 0;
			::swprintf_s(s1,13,L"%05u‖文本＝",指针号);
			s1 += 9;
			BOOL 不存在重复 = TRUE;
			s64 本文本地址 = m_指向文本的地址[指针号];
			for( UINT i = 0 ; i < 指针号; ++i )
			{
				if( 本文本地址 == m_指向文本的地址[i] )
				{
					WCHAR 重复TXT[20];
					不存在重复 = FALSE;
					::swprintf_s(重复TXT,19,L"{重复符：%05u}",i);
					s1 += ::WQSG_strcpy (重复TXT,s1);
					break;
				}
			}
			if(不存在重复)
			{
				while(TRUE)
				{
					////////////////////////
					UINT 实际III = mFile_ROM.GetUCHAR( (UINT)m_MAX_Byte , 缓冲 );
					if( 0 == 实际III )
						break;
					//////////////////////匹配
					WCHAR* tmp = NULL;//改成匹配

					UINT 索引长度;
					tmp = m_Text.匹配( 缓冲 , 实际III , 索引长度 );
					/////////////////
					if(tmp)
					{
						长度+= 索引长度;
						mFile_ROM.BACK(实际III - 索引长度);
						if(*((u64*)tmp) == m_OVER)
						{
							s1 += ::WQSG_strcpy(L"{结束符",s1);
							for(UINT i = 0;i < 索引长度;i++)
							{
								::swprintf_s((s1++),03,L"%02X",缓冲[i]);
								s1++;
							}
							*(s1++) = L'}';
							break;
						}
						else if(*((u64*)tmp) == m_TURN)
						{
							s1 += ::WQSG_strcpy(L"{跳跃符",s1);
							for(UINT i = 0;i < 索引长度;i++)
							{
								::swprintf_s((s1++),3,L"%02X",缓冲[i]);
								s1++;
							}
							*(s1++) = L'：';
							///////////////
							tmp = ::WQSG_getstrL(tmp + 4,-1);
							::swscanf_s(tmp,L"%d",&实际III);
							delete[]tmp;

							索引长度= mFile_ROM.GetUCHAR(实际III,缓冲);
							长度+= 索引长度;
							for(UINT i = 0;i < 索引长度;i++)
							{
								::swprintf_s((s1++),3,L"%02X",缓冲[i]);
								s1++;
							}
							*(s1++) = L'}';
						}
						else
						{
							s1 += ::WQSG_strcpy(tmp,s1);
						}
					}
					else
					{// 没找到
						mFile_ROM.BACK(--实际III);
						//未找到的异常处理
						*(s1++) = L'{';
						::swprintf_s(s1++,3,L"%02X",*缓冲);
						*(++s1) = L'}';
						s1++;
						长度++;
					}
				}
			}
			//写出文本
			::WQSG_strcpy (L"\r\n\r\n",s1);
			if( !a_fp.WriteStrW( BUF_OUT ) )
			{
				zzz_Log( L"写出文本失败" , ROM_path );
				return FALSE;
			}
		}
		return TRUE;
	}
	void 清空指针表()
	{
		delete[]m_指向文本的地址;
		m_指向文本的地址 = NULL;
		m_指针个数 = 0;
	}
private:
	BOOL		已载入码表;
	WCHAR*		BUF_OUT;
	///////////////////////////////////
	s64*		m_指向文本的地址;
	u32			m_指针个数;
	///////////////////////
public:
	WQSG_pTXT_O( )
		:CWQSG_MapTbl_OUT( )
		,m_指针个数(0)
		,已载入码表(FALSE)
		,m_指向文本的地址(NULL)
	{
		BUF_OUT = new WCHAR[WQSG_TXT_BUF + 1];
	}
	virtual	~WQSG_pTXT_O()
	{
		delete BUF_OUT;
	}
	void 清空码表(void)
	{
		m_MAX_Byte = 0;
		m_Text.清空();

		已载入码表 = FALSE;
	}
	BOOL 载入码表( WCHAR* a_pPathName , WCHAR* TBL2_path , int a_iCheckTbl = 0 , BOOL 需要结束符 = FALSE )
	{
		m_bCheckTbl = (a_iCheckTbl != 0);
		清空码表();
		if(LOAD_TBL(a_pPathName,m_bCheckTbl))
		{
			if(需要结束符)
			{
				if(*TBL2_path)
				{
					if(LOAD_TBL2(TBL2_path,m_bCheckTbl,FALSE))
					{
						已载入码表= TRUE;
						return TRUE;
					}
				}
				zzz_Log( L"此模式必须指定一个含有结束符的控制码表" );
			}
			else
			{
				已载入码表= TRUE;
				return TRUE;
			}
		}
		清空码表();
		return FALSE;
	}
	BOOL 地址_指针导出(WCHAR* rom_path,
		WCHAR * TXT_path,
		INT 验证,
		s64 首指针地址,
		s64 最后指针地址,
		u8 指针间隔,
		u8 指针实长,
		u32 文本基础地址= (u32)0,
		u8 指针倍率= (u8)1
		)
	{/////////////////////////////////////////////////////////////////
		if(!已载入码表)
		{
			zzz_Log( L"请先载入码表" , rom_path );
			return FALSE;
		}
		if( 首指针地址 < 0 )
		{
			zzz_Log( L"首指针地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
			return FALSE;
		}
		if( 最后指针地址 < 0 )
		{
			zzz_Log( L"最后指针地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
			return FALSE;
		}

		*BUF_OUT = 0;
		m_bCheckTbl= (验证!= 0);

		if(首指针地址 >= 最后指针地址)
		{
			zzz_Log( L"指针表结束地址 必须大于 指针表首项地址" , rom_path );
			return FALSE;
		}
		if(0 == 指针实长)
		{
			zzz_Log( L"指针实长不能为0" , rom_path );
			return FALSE;
		}
		if(指针实长> 8)
		{
			zzz_Log( L"指针实长不能超过8字节" , rom_path );
			return FALSE;
		}

		if(指针间隔< 指针实长)
		{
			zzz_Log( L"表项长度不能小于指针实长" , rom_path );
			return FALSE;
		}
		if(0 == 指针倍率)
		{
			zzz_Log( L"指针倍率不能为0" , rom_path );
			return FALSE;
		}
		u64 指针个数= (最后指针地址- 首指针地址) / 指针间隔 + 1;
		if(指针个数 > 99999)
		{
			zzz_Log( L"安全起见,指针个数不能超过 99999" , rom_path );
			return FALSE;
		}
////////////////////////////////////////////////////////////////
		::WQSG_FM_FILE mFile_ROM;
		if(!mFile_ROM.OpenFile(rom_path,3))
		{
			zzz_Log( L"打开ROM文件失败" , rom_path );
			return FALSE;
		}
		if(最后指针地址 >= mFile_ROM.Get文件长度())
		{
			zzz_Log( L"指针表结束地址不存在,超出文件大小" , rom_path );
			return FALSE;
		}

		清空指针表();
		m_指针个数= (u32)指针个数;
		///////////////////////////////////////////////////////////////
		::CWQSG_File fp;

		if( !fp.OpenFile( TXT_path , 4 , 3 ) )
		{
			zzz_Log( L"创建文本文件失败" , rom_path );
			return FALSE;
		}
		if( !读入指针表(rom_path , mFile_ROM , 首指针地址 , 指针实长 , 指针间隔 , 指针倍率 , 文本基础地址) )
			return FALSE;

		::swprintf_s(BUF_OUT,WQSG_TXT_BUF,L"‖‖‖‖‖01‖‖‖‖‖%08X‖‖‖‖‖%08X‖‖‖‖‖%08X‖‖‖‖‖%02u‖‖‖‖‖%02u‖‖‖‖‖%02u‖‖‖‖‖\r\n\r\n\r\n",
			(u32)首指针地址 , m_指针个数 , 文本基础地址 ,指针间隔 , 指针实长 , 指针倍率 );

//		CHAR AAA[65535];	::sprintf(AAA,"‖‖‖‖‖01‖‖‖‖‖%08X‖‖‖‖‖%08X‖‖‖‖‖%08X",(U32)首指针地址,文本基础地址,文件头长度);
		if( 地址_正式导出( rom_path , fp , mFile_ROM ) )
		{
			清空指针表();
			zzz_Log( L"导出完毕" , rom_path );
			return TRUE;
		}
		清空指针表();
		return FALSE;
	}
	BOOL 地址_长度_指针导出(WCHAR* rom_path,
		WCHAR * TXT_path,
		INT 验证,
		s64 首指针地址,
		s64 最后指针地址,
		u8 指针间隔,
		u8 指针实长,
		u8 地址长度,
		u32 文本基础地址= (u32)0,
		u8 指针倍率= (u8)1,
		u8 长度块倍率= (u8)1,
		u8 长度块长度字节= (u8)2
		)
	{
		return FALSE;
	}
};
#undef WQSG_TXT_BUF
//----------------------------------------------------------------------------

#endif