#ifndef __Alternative_h__
#define __Alternative_h__
/************************************************************************************
*	Zaklad pre getDword,getWord,getByte,getQword
*	Parametre:
*		Vystup
*		Zaklad
*		Odchylka
*		Velkost (qword/dword/word/byte)
*		Vystupny register (rcx/ecx/cx/ch/cl)
*		Vlastne[segment] (ds:,ss:,cs:)
*	GIAO vs GIAO_DIR
*		v GIAO,		zaklad = premenna
*		v GIAO_DIR,	zaklad = adresa
*************************************************************************************/
#define giao(out,base,off,tp,rg,vl)\
	_asm{\
		__asm lea ebx,base\
		__asm mov rg,tp ptr vl[ebx+off]\
		__asm mov out,rg\
	}
#define giao_dir(out,base,off,tp,rg,vl)\
	_asm{\
		__asm mov ebx,base\
		__asm mov rg,tp ptr vl[ebx+off]\
		__asm mov out,rg\
	}
/************************************************************************************
*	Odvodenia pre giao[_dir] s automatickym dosadenim velkosti,registra a segmentu
*************************************************************************************/
#define getDword(out,base,offset) giao(out,base,offset,dword,ecx,ds:)
#define getWord(out,base,offset) giao(out,base,offset,word,cx,ds:)
#define getByte(out,base,offset) giao(out,base,offset,byte,ch,ds:)
#define getQword(out,base,offset) giao(out,base,offset,qword,rcx,ds:)
#define getDword_dir(out,base,offset) giao_dir(out,base,offset,dword,ecx,ds:)
#define getWord_dir(out,base,offset) giao_dir(out,base,offset,word,cx,ds:)
#define getByte_dir(out,base,offset) giao_dir(out,base,offset,byte,ch,ds:)
#define getQword_dir(out,base,offset) giao_dir(out,base,offset,qword,rcx,ds:)
/* Argumenty sa push-uju sprava dolava */
#define pushParam(p) __asm push p;
/*************************************************************************************
	Volanie metod urciteho objektu
	Parametre pre callMethod:
		Objekt
		Adresa funkcie
	Parametre pre callMethodRet:
		Vystup
		Objekt
		Adresa funkcie
	callMethod vs callMethod_dir:
		callMethod     - objekt = premenna
		callMethod_dir - objekt	= adresa
*************************************************************************************/
#define callMethod(obj,addr) __asm{\
	__asm lea ecx,obj\
	__asm mov edx,addr\
	__asm call edx\
}
#define callMethodRet(res,obj,addr) __asm{\
	__asm lea ecx,obj\
	__asm mov edx,addr\
	__asm call edx\
	__asm mov res,eax\
}
#define callMethod_dir(obj,addr) __asm{\
	__asm mov ecx,obj\
	__asm mov edx,addr\
	__asm call edx\
}
#define callMethodRet_dir(res,obj,addr) __asm{\
	__asm mov ecx,obj\
	__asm mov edx,addr\
	__asm call edx\
	__asm mov res,eax\
}
#endif