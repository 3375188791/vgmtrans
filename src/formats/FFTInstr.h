#pragma once
#include "VGMInstrSet.h"
#include "VGMSampColl.h"
#include "VGMRgn.h"
#include "FFTFormat.h"
#include "PS1Format.h"
#include "Matcher.h"



/****************************************************************/
/*																*/
/*					Define of structs							*/
/*																*/
/****************************************************************/
//==============================================================
//		Header of wds
//--------------------------------------------------------------
//	Memo:
//			Offset Address	= 0x0000
//			Size			= 0x0030
//--------------------------------------------------------------
struct WdsHdr
{
	unsigned	long	sig;				//strings "wds "
				long	unknown_0004;
	unsigned	long	iFFT_InstrSetSize;	//size of entire instrset (header + instrs + samps)
											//but FFT only, so we ignore it
				long	unknown_000C;
	unsigned	long	szHeader1;			//size of header???
	unsigned	long	szSampColl;			//size of AD-PCM body (.VB) ???
	unsigned	long	szHeader2;			//size of header???
	unsigned	long	iNumInstrs;			//Quantity of instruments.
	unsigned	long	iBank;				//Bank No.
				long	unknown_0024;
				long	unknown_0028;
				long	unknown_002C;
};



//==============================================================
//		Attribute of wds
//--------------------------------------------------------------
//	Memo:
//			Offset Address	= 0x0030
//			Size			= 0x0010 �~ iQuantity
//--------------------------------------------------------------
struct WdsRgnData
{
	unsigned	long	ptBody;							//Offset address to AD-PCM �g�`����
	unsigned	short	ptLoop;							//size?  loop?  unknown
	unsigned	char	iFineTune;		// Pitch table is at 800290D8 in FFT.  See function at 80017424
										//  takes $a0: uint16_t- MSB = semitone (note+semitone_tune),
										//                  LSB = fine tune index
				char	iSemiToneTune;	// Pitch tune in semitones (determines unitykey)
	unsigned	char	Ar;				// & 0x7F attack rate
	unsigned	char	Dr;				// & 0x0F decay rate
	unsigned	char	Sr;				// & 0x7F sustain rate 
	unsigned	char	Rr;				// & 0x1F release rate
	unsigned	char	Sl;				// & 0x0F sustain level
	unsigned	char	Am;				// & 0x01 attack rate linear (0)or exponential(1)  UNSURE
	unsigned	char	unk_E;
	unsigned	char	unk_F;
};





/****************************************************************/
/*																*/
/*					Define of structs							*/
/*																*/
/****************************************************************/
//==============================================================
//		Instrument Set		(Bank�S��)
//--------------------------------------------------------------
class WdsInstrSet :
	public VGMInstrSet
{
public:
	WdsInstrSet(RawFile* file, ULONG offset);
	virtual ~WdsInstrSet(void);

	virtual bool	GetHeaderInfo();	//�w�b�_�[�̏���
										//�����ŁAObject"VabSampColl"�𐶐�����ׂ��H
										//�i�X�L���i�[��VB����������ƁA�����L�邩�����Ȃ��Ȃ�B�j
	virtual bool	GetInstrPointers();	//���FObject"WdsInstr"�𐶐�����B
										//"aInstrs"�ɁA�o�^����B
										//�e���F���̏���

	enum Version { VERSION_DWDS, VERSION_WDS };

public:
	WdsHdr		hdr;
	Version version;

/*	member of "VGMInstrSet"
	VGMInstrSet::aInstrs		//���F����vector
	VGMInstrSet::dls			//class DLS�����p
	VGMInstrSet::menu			//
	VGMInstrSet::sampColl		//�g�`���̂̃I�u�W�F�N�g
*/
};



//==============================================================
//		Program information			( 1 Instrument)
//--------------------------------------------------------------
class WdsInstr
	: public VGMInstr
{
public:
	WdsInstr(VGMInstrSet* instrSet, ULONG offset, ULONG length, ULONG theBank, ULONG theInstrNum);
	virtual ~WdsInstr(void);

	virtual bool LoadInstr();	//Object "WdsRgn"�̐����A
								//"WdsRgn->LoadRgn()"�̌Ăяo��
								//member "aRgns" �փI�u�W�F�N�g�̃|�C���^��o�^

public:
	WdsRgnData rgndata;

/*	member of "VGMInstr"
	VGMInstr::aRgns				// 
	VGMInstr::bank				// bank number
	VGMInstr::instrNum			// program number
	VGMInstr::parInstrSet		// 
*/
	
};
