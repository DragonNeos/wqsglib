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
#pragma once
#include "WQSG_def.h"
#include <windows.h>
__i__
__i__
__i__
__i__
class CWD_BIN
{
public:
	u8*		BIN;
	u32		LEN;
	inline	CWD_BIN(void):BIN(NULL),LEN(0){}
	virtual	inline	~CWD_BIN(void){delete[]BIN;}
	inline	u8*	SetBufSize( u32 size )
	{
		delete[]BIN;
		return ( BIN = new u8[size] );
	}
};
//------------------------------------------------------------------------------
class WD_TXTW
{
	WCHAR*	m_TXT;
public:
	WD_TXTW():m_TXT(NULL){}
	virtual	~WD_TXTW(){delete[]m_TXT;}
	WCHAR* operator	=( WCHAR* TXT ){		delete[]m_TXT;		m_TXT = TXT;		return m_TXT;	}
	void	����(){		m_TXT = NULL;	}
	operator WCHAR*	(){		return m_TXT;	}
};
//------------------------------------------------------------------
#ifdef __WD_str_LB_W__
class WD_str_LB_W{
#define WQSG_BUFlen 512
	WCHAR*		TXT;
	WD_str_LB_W*	next;
	void z_Copy(WCHAR*s){
		s += WQSG_strcpy(TXT,s);
		if(next)
			next->z_Copy(s);
	}
public:
	WD_str_LB_W(){
		TXT = new WCHAR[WQSG_BUFlen + 1];
		next = NULL;
	}
	~WD_str_LB_W(){
		delete TXT;
		if(next)
			delete next;
	}
	INT ReadTXTLine(HANDLE FILE_ID)//���ļ������ı�,�������� ���� ,���� ���� �� �ļ����� ���� WD_str_LB*,������ Ϊ0,����NULL;
	{
		WCHAR C[1],*TXT_BUF = TXT;
		DWORD con_i;INT i;
		for(i = 0 ; i < WQSG_BUFlen ; i+=2 )
		{
			if(::ReadFile(FILE_ID,C,2,&con_i,NULL) && 2 == con_i)//������
			{
				if(2 != con_i)
					break;
				if(0x0D == *C)//�ж��Ƿ���,�Ǿͷ���
				{
					i+=4;
					if(::ReadFile(FILE_ID,C,2,&con_i,NULL) && 2 == con_i)////////////////////////////////////////������
					{
						if(0x0A != *C)//�ǻ��к�׺
						{
							::SetFilePointer(FILE_ID,-2,NULL,FILE_CURRENT);
							i-=2;
						}
					}
					else//����ʧ��
					{
						i-=2;
					}
					*TXT_BUF = 0;
					if(i>0)//������������
						return i;
					return 0;//������,���ؼ�
				}
				*(TXT_BUF++) = *C;
			}////////////////////////////////////////////////////////////////////////////////������ ����
			else/////////////////////////////////////////��ʧ��,�������ļ�����
			{
				*TXT_BUF = 0;
				if(i>0)//������������
					return i;
				return 0;//������,���ؼ�
			}///////////////////////////////////////////��ʧ�� ����
		}
		////////////////////////////////////////////����������
		*TXT_BUF = 0;
		next = new WD_str_LB_W;
		int x =	next->ReadTXTLine(FILE_ID);
		if(!x)
		{
			delete next;
			next=NULL;
		}
		return (i+x);
	}

	UINT Getlen()
	{
		UINT i = WQSG_strlen(TXT);
		if(next)
			i += next->Getlen();
		return i;
	}
	WCHAR*GetTXT()
	{
		WCHAR*out_0=new WCHAR[Getlen() + 1];
		z_Copy(out_0);
		return out_0;
	}
#undef WQSG_BUFlen// 512
};
#endif
#if 0
//------------------------------------------------------------------
class �ڵ�_TXT_W
{
public:
	WCHAR*			m_TXT_W;
	�ڵ�_TXT_W*		m_next[256];
public:
	�ڵ�_TXT_W()
	{
		m_TXT_W = NULL;
		::memset(m_next,0,sizeof(m_next));
	}
	virtual	~�ڵ�_TXT_W()
	{
		delete[]m_TXT_W;
		for( INT I = 0 ; I <= 255 ; ++I )
			delete m_next[I];
	}
};
#endif
//------------------------------------------------------------------
template <typename TYPE_1>
class CWD_tree_del
{
	class __Node
	{
	public:
		TYPE_1*	m_Type;
		__Node*	m_next[256];
		///////////////////////////////////////////////////////////////////
		inline __Node(): m_Type ( NULL ){	::memset( m_next , 0 , sizeof(m_next) );}
		virtual	inline ~__Node(){	delete m_Type;	for( INT I = 0 ; I <= 255 ; ++I )delete m_next[I];	}
	};
	__Node*	m_ROOB[256];
public:
	inline	CWD_tree_del(){	::memset( m_ROOB , 0 , sizeof(m_ROOB) );	}
	virtual	inline	~CWD_tree_del()	{		for( INT k = 0 ; k <= 255 ; ++k )delete m_ROOB[k];	}
	///////////////////////////////////////////////////////////////////
	inline	void		���()
	{
		for(INT k = 0;k <= 255;++k)
			delete m_ROOB[k];
		::memset(m_ROOB,0,sizeof(m_ROOB));
	}
	inline	BOOL		ѹ��( UCHAR* ���� , INT �������� , TYPE_1* ���ӵ�Type )
	{
		if( ( �������� <= 0 ) || ( ���� == NULL ) )
			return FALSE;

		__Node** �ڵ��� = m_ROOB;
		__Node* tmp;
		INT x;

		while(��������--)
		{
			x = *(����++);
			tmp = �ڵ���[x];
			if(tmp)
			{//����,����������һ��
				�ڵ��� = tmp->m_next;
			}
			else
			{//������,�����µ� �ڵ�
				tmp = new __Node;				//::memset(tmp,0,sizeof(�ڵ�_TXT_W));
				�ڵ���[x] = tmp;
				�ڵ��� = tmp->m_next;
			}
		}
		tmp->m_Type = ���ӵ�Type;

		return TRUE;
	}
	inline	TYPE_1*		ƥ��( u8* ���� , UINT �������� , UINT& ʵ��ƥ�䳤�� )const
	{
		if( ( �������� <= 0 ) || ( ���� == NULL ) )
			return NULL;

		ʵ��ƥ�䳤�� = ��������;
		__Node*const* �ڵ��� = m_ROOB;
		const __Node* tmp;

		u32 xlen = 0;
		TYPE_1*	xtmp = NULL;
		while(��������--)
		{
			tmp = �ڵ���[*(����++)];
			if(tmp)
			{//������һ��
				if( tmp ->m_Type )
				{
					xtmp = tmp ->m_Type;
					xlen = ��������;
				}
				�ڵ��� = tmp->m_next;
			}
			else if( xtmp )//��������һ��
			{
				�������� = --xlen;
				break;
			}
			else
				break;
		}
		ʵ��ƥ�䳤�� -= (�������� + 1);
		return xtmp;
	}
	inline	TYPE_1*		����( u8* ���� , INT �������� )const
	{
		if( ( �������� <= 0 ) || ( ���� == NULL ) )
			return NULL;

		__Node*const* �ڵ��� = m_ROOB;
		const __Node* tmp;

		while(��������--)
		{
			tmp = �ڵ���[*(����++)];
			if(tmp)//������һ��
				�ڵ��� = tmp->m_next;
			else//��������һ��
				return NULL;
		}
		return tmp->m_Type;
	}
};
//------------------------------------------------------------------
template <typename TYPE_1>
class CWD_tree
{
	class __Node
	{
	public:
		TYPE_1	m_Type;
		__Node*	m_next[256];
		///////////////////////////////////////////////////////////////////
		inline __Node(): m_Type ( 0 ){::memset( m_next , 0 , sizeof(m_next) );}
		virtual	inline ~__Node()	{	for( INT I = 0 ; I <= 255 ; ++I )delete m_next[I];		}
	};
	__Node*	m_ROOB[256];
public:
	inline	CWD_tree(){	::memset( m_ROOB , 0 , sizeof(m_ROOB) );	}
	virtual	inline	~CWD_tree()	{		for( INT k = 0 ; k <= 255 ; ++k )delete m_ROOB[k];	}
	///////////////////////////////////////////////////////////////////
	inline	void		���()
	{
		for(INT k = 0;k <= 255;++k)
			delete m_ROOB[k];
		::memset(m_ROOB,0,sizeof(m_ROOB));
	}
	inline	BOOL		ѹ��( UCHAR* ���� , INT �������� , TYPE_1 ���ӵ�Type )
	{
		if( ( �������� <= 0 ) || ( ���� == NULL ) )
			return FALSE;

		__Node** �ڵ��� = m_ROOB;
		__Node* tmp;
		INT x;

		while(��������--)
		{
			x = *(����++);
			tmp = �ڵ���[x];
			if(tmp)
			{//����,����������һ��
				�ڵ��� = tmp->m_next;
			}
			else
			{//������,�����µ� �ڵ�
				tmp = new __Node;				//::memset(tmp,0,sizeof(�ڵ�_TXT_W));
				�ڵ���[x] = tmp;
				�ڵ��� = tmp->m_next;
			}
		}
		tmp->m_Type = ���ӵ�Type;

		return TRUE;
	}
	inline	TYPE_1		ƥ��( u8* ���� , INT �������� , INT& ʵ��ƥ�䳤�� )
	{
		if( ( �������� <= 0 ) || ( ���� == NULL ) )
			return NULL;

		ʵ��ƥ�䳤�� = ��������;
		__Node** �ڵ��� = m_ROOB,
		__Node* tmp;

		u32 xlen = 0;
		TYPE_1	xtmp = 0;
		while(��������--)
		{
			tmp = �ڵ���[*(����++)];
			if(tmp)
			{//������һ��
				if( tmp ->m_Type )
				{
					xtmp = tmp ->m_Type;
					xlen = ��������;
				}
				�ڵ��� = tmp->m_next;
			}
			else if( xtmp )//��������һ��
				�������� = --xlen;
			else
				break;
		}
		ʵ��ƥ�䳤�� -= (�������� + 1);
		return xtmp;
	}
	inline	TYPE_1		����( u8* ���� , INT �������� )
	{
		if( ( �������� <= 0 ) || ( ���� == NULL ) )
			return NULL;

		__Node** �ڵ��� = m_ROOB;
		__Node* tmp;

		while(��������--)
		{
			tmp = �ڵ���[*(����++)];
			if(tmp)//������һ��
				�ڵ��� = tmp->m_next;
			else//��������һ��
				return 0;
		}
		return tmp->m_Type;
	}
};