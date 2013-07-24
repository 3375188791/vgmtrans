#include "stdafx.h"
#include "FFTFormat.h"
#include "FFTInstr.h"
#include "Vab.h"
#include "VGMSamp.h"
#include "PSXSPU.h"

using namespace std;

/****************************************************************/
/*																*/
/*			Instrument Set		(Bank�S��)						*/
/*																*/
/****************************************************************/
//==============================================================
//		Constructor
//--------------------------------------------------------------
WdsInstrSet::WdsInstrSet(RawFile* file, ULONG offset):
	VGMInstrSet(FFTFormat::name, file, offset)
{
}

//==============================================================
//		Destructor
//--------------------------------------------------------------
WdsInstrSet::~WdsInstrSet(void)
{
}

//==============================================================
//		�w�b�_�[���̎擾
//--------------------------------------------------------------
//	Memo:
//		VGMInstrSet::Load()�֐�����Ă΂��
//==============================================================
int	WdsInstrSet::GetHeaderInfo()
{

	//"hdr"�\���̂ւ��̂܂ܓ]��
	GetBytes(dwOffset, sizeof(WdsHdr), &hdr);
	unLength	= hdr.szHeader1 + hdr.szSampColl;	//header size + samp coll size
	id			= hdr.iBank;						//Bank number.

	if (hdr.sig == 0x73647764)
		version = VERSION_DWDS;
	else if (hdr.sig == 0x20736477)
		version = VERSION_WDS;

	//�o�C�i���G�f�B�^�\���p
	wostringstream	theName;
	theName << L"wds " << id;
	name = theName.str();

	//�w�b�_�[object�̐���
	VGMHeader* wdsHeader = AddHeader(dwOffset, sizeof(WdsHdr));
	wdsHeader->AddSig(dwOffset, sizeof(long));
	wdsHeader->AddUnknownItem(dwOffset+0x04, sizeof(long));
	wdsHeader->AddSimpleItem(dwOffset+0x08, sizeof(long), L"Header size? (0)");
	wdsHeader->AddUnknownItem(dwOffset+0x0C, sizeof(long));
	wdsHeader->AddSimpleItem(dwOffset+0x10, sizeof(long), L"Header size? (1)");
	wdsHeader->AddSimpleItem(dwOffset+0x14, sizeof(long), L"AD-PCM body(.VB) size");
	wdsHeader->AddSimpleItem(dwOffset+0x18, sizeof(long), L"Header size? (2)");
	wdsHeader->AddSimpleItem(dwOffset+0x1C, sizeof(long), L"Number of Instruments");
	wdsHeader->AddSimpleItem(dwOffset+0x20, sizeof(long), L"Bank number");
	wdsHeader->AddUnknownItem(dwOffset+0x24, sizeof(long));
	wdsHeader->AddUnknownItem(dwOffset+0x28, sizeof(long));
	wdsHeader->AddUnknownItem(dwOffset+0x2C, sizeof(long));

	//�g�`object�̐���
	sampColl = new PSXSampColl(FFTFormat::name, this, dwOffset + hdr.szHeader1, hdr.szSampColl);
//	sampColl->Load();				//VGMInstrSet::Load()�֐����ł���Ă���B
//	sampColl->UseInstrSet(this);	//"WD.cpp"�ł́A���l�̎�������Ă���B

	return true;

}

//==============================================================
//		�e���F�̏��擾
//--------------------------------------------------------------
//	Memo:
//		VGMInstrSet::Load()�֐�����Ă΂��
//==============================================================
int	WdsInstrSet::GetInstrPointers()
{

	ULONG	iOffset = dwOffset + sizeof(WdsHdr);	//pointer of attribute table

	//���F�������J��Ԃ��B
	for(unsigned int i=0; i<=hdr.iNumInstrs; i++)
	{
		//WdsInstr* newInstr = new WdsInstr(this,iOffset,sizeof(WdsRgnData),hdr.iBank,i);		//0 �c hdr.iBank
		WdsInstr* newInstr = new WdsInstr(this,iOffset,sizeof(WdsRgnData), i/128 , i%128);		//0 �c hdr.iBank
		aInstrs.push_back(newInstr);
	//	newInstr->LoadInstr();		//VGMInstrSet::Load()�֐����iLoadInstrs()�j�ł���Ă���B
		iOffset += sizeof(WdsRgnData);	// size = 0x0010
	}

	return true;
}



/****************************************************************/
/*																*/
/*			Program information			( 1 Instrument)			*/
/*																*/
/****************************************************************/
//==============================================================
//		Constructor
//--------------------------------------------------------------
WdsInstr::WdsInstr(VGMInstrSet* instrSet, ULONG offset, ULONG length, ULONG theBank, ULONG theInstrNum):
	VGMInstr(instrSet, offset, length, theBank, theInstrNum)
{
}

//==============================================================
//		Destructor
//--------------------------------------------------------------
WdsInstr::~WdsInstr(void)
{
}

//==============================================================
//		Make the Object "WdsRgn" (Attribute table)
//--------------------------------------------------------------
int	WdsInstr::LoadInstr()
{

//	WdsRgn* rgn = new WdsRgn(this, dwOffset, sizeof(WdsAttr), instrNum, parInstrSet->sampColl);
	//WdsRgn* rgn = new WdsRgn(this, dwOffset);
	//if (!rgn->LoadRgn())
	//	return false;
	//aRgns.push_back(rgn);

	////Object "VGMRgn"��"VGMSampColl"�̊֘A�t���H
	//rgn->sampNum		= instrNum;					//Wave number.
//	rgn->sampCollPtr	= parInstrSet->sampColl;	//NDS�p�H�iInstSet�ɁASampColl�������L��悤��format�j

	GetBytes(dwOffset, sizeof(WdsRgnData), &rgndata);
	VGMRgn* rgn = new VGMRgn(this, dwOffset, unLength);
	rgn->sampOffset =		rgndata.ptBody;
	if (((WdsInstrSet*)parInstrSet)->version == WdsInstrSet::VERSION_WDS)
		rgn->sampOffset *= 8;
	//rgn->loop.loopStart =	rgndata.ptLoop;
	rgn->unityKey =			0x3C - rgndata.iSemiToneTune;
	//an iFineTune value of 256 should equal 100 cents, and linear scaling seems to do the trick.
	// see the declaration of iFineTune for info on where to find the actual code and table for this in FFT
	rgn->fineTune =			(short)((double)rgndata.iFineTune * (100.0/256.0));	

	PSXConvADSR(rgn, rgndata.Am > 1, rgndata.Ar, rgndata.Dr, rgndata.Sl, 1, 1, rgndata.Sr, 1, rgndata.Rr, false);
	aRgns.push_back(rgn);

	rgn->AddGeneralItem(dwOffset+0x00, sizeof(U32), L"Sample Offset");
	rgn->AddGeneralItem(dwOffset+0x04, sizeof(U16), L"Loop Offset");
	rgn->AddGeneralItem(dwOffset+0x06, sizeof(U16), L"Pitch Fine Tune");
	rgn->AddGeneralItem(dwOffset+0x08, sizeof(U8), L"Attack Rate");
	rgn->AddGeneralItem(dwOffset+0x09, sizeof(U8), L"Decay Rate");
	rgn->AddGeneralItem(dwOffset+0x0A, sizeof(U8), L"Sustain Rate");
	rgn->AddGeneralItem(dwOffset+0x0B, sizeof(U8), L"Release Rate");
	rgn->AddGeneralItem(dwOffset+0x0C, sizeof(U8), L"Sustain Level");
	rgn->AddGeneralItem(dwOffset+0x0D, sizeof(U8), L"Attack Rate Mode?");
	rgn->AddGeneralItem(dwOffset+0x0E, sizeof(U8), L"unknown");
	rgn->AddGeneralItem(dwOffset+0x0F, sizeof(U8), L"unknown");

	return TRUE;
}
