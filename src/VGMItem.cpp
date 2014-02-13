#include "stdafx.h"
#include "VGMItem.h"
#include "RawFile.h"
#include "VGMFile.h"
#include "Root.h"

using namespace std;

VGMItem::VGMItem()
: color(0)
{
}

VGMItem::VGMItem(VGMFile* thevgmfile, uint32_t theOffset, uint32_t theLength, const wstring theName, uint8_t theColor)
: vgmfile(thevgmfile),
  name(theName),
  dwOffset(theOffset),
  unLength(theLength),
  color(theColor)
{
}

VGMItem::~VGMItem(void)
{
}

bool operator> (VGMItem &item1, VGMItem &item2)
{
	return item1.dwOffset > item2.dwOffset;
}

bool operator<= (VGMItem &item1, VGMItem &item2)
{
	return item1.dwOffset <= item2.dwOffset;
}

bool operator< (VGMItem &item1, VGMItem &item2)
{
	return item1.dwOffset < item2.dwOffset;
}

bool operator>= (VGMItem &item1, VGMItem &item2)
{
	return item1.dwOffset >= item2.dwOffset;
}


RawFile* VGMItem::GetRawFile()
{
	return vgmfile->rawfile;
}


VGMItem* VGMItem::GetItemFromOffset(uint32_t offset)
{
	if (IsItemAtOffset(offset))
		return this;
	return NULL;
}

void VGMItem::AddToUI(VGMItem* parent, VOID* UI_specific)
{
	pRoot->UI_AddItem(this, parent, name, UI_specific);
}

/*
void SetOffset(uint32_t newOffset)
{
	dwOffset = newOffset;
}

void SetLength(uint32_t newLength)
{
	unLength = newLength;
}*/

uint32_t VGMItem::GetBytes(uint32_t nIndex, uint32_t nCount, void* pBuffer)
{
	return vgmfile->GetBytes(nIndex, nCount, pBuffer);
}

uint8_t VGMItem::GetByte(uint32_t offset)
{
	return vgmfile->GetByte(offset);
}

uint16_t VGMItem::GetShort(uint32_t offset)
{
	return vgmfile->GetShort(offset);
}

uint32_t VGMItem::GetWord(uint32_t offset)
{
	return GetRawFile()->GetWord(offset);
}

//GetShort Big Endian
uint16_t VGMItem::GetShortBE(uint32_t offset)
{
	return GetRawFile()->GetShortBE(offset);
}

//GetWord Big Endian
uint32_t VGMItem::GetWordBE(uint32_t offset)
{
	return GetRawFile()->GetWordBE(offset);
}

bool VGMItem::IsValidOffset(uint32_t offset)
{
	return vgmfile->IsValidOffset(offset);
}



//  ****************
//  VGMContainerItem
//  ****************

VGMContainerItem::VGMContainerItem()
: VGMItem()
{
	AddContainer(headers);
	AddContainer(localitems);
}


VGMContainerItem::VGMContainerItem(VGMFile* thevgmfile, uint32_t theOffset, uint32_t theLength, const wstring theName, uint8_t color)
: VGMItem(thevgmfile, theOffset, theLength, theName, color)
{
	AddContainer(headers);
	AddContainer(localitems);
}

VGMContainerItem::~VGMContainerItem()
{
	DeleteVect(headers);
	DeleteVect(localitems);
}

VGMItem* VGMContainerItem::GetItemFromOffset(uint32_t offset)
{
	if (IsItemAtOffset(offset))				//if the offset is within this item
	{
		for (uint32_t i=0; i<containers.size(); i++)
		{
			for (uint32_t j=0; j<containers[i]->size(); j++)	
			{
				VGMItem* foundItem = (*containers[i])[j]->GetItemFromOffset(offset);
				if (foundItem)
					return foundItem;
			}
		}
		return NULL; // this offset must be a "hole", so that it should return nothing
	}
	return NULL;
}

void VGMContainerItem::AddToUI(VGMItem* parent, VOID* UI_specific)
{
	VGMItem::AddToUI(parent, UI_specific);
	for (uint32_t i=0; i<containers.size(); i++)
	{
		for (uint32_t j=0; j<containers[i]->size(); j++)
			(*containers[i])[j]->AddToUI(this, UI_specific);
	}
}

VGMHeader* VGMContainerItem::AddHeader(uint32_t offset, uint32_t length, const wchar_t* name)
{
	VGMHeader* header = new VGMHeader(this, offset, length, name);
	headers.push_back(header);
	return header;
}

void VGMContainerItem::AddItem(VGMItem* item)
{
	localitems.push_back(item);
}

void VGMContainerItem::AddSimpleItem(uint32_t offset, uint32_t length, const wchar_t *name)
{
	localitems.push_back(new VGMItem(this->vgmfile, offset, length, name, CLR_HEADER));
	//items.push_back(new VGMHeaderItem(this, VGMHeaderItem::HIT_GENERIC, offset, length, name));
}

void VGMContainerItem::AddUnknownItem(uint32_t offset, uint32_t length)
{
	localitems.push_back(new VGMItem(this->vgmfile, offset, length, L"Unknown"));
	//items.push_back(new VGMHeaderItem(this, VGMHeaderItem::HIT_UNKNOWN, offset, length, L"Unknown"));
}

/*
void VGMContainerItem::AddHeaderItem(VGMItem* item)
{
	header->AddItem(item);
}
	
void VGMContainerItem::AddSimpleHeaderItem(uint32_t offset, uint32_t length, const wchar_t* name)
{
	header->AddSimpleItem(offset, length, name);
}
*/


//VGMContainerItem::~VGMContainerItem(void)
//{
//	//for (uint32_t i=0; i<containers.size(); i++)
//	//	DeleteVect<VGMItem>(*containers[i]);
//		//DeleteVect<VGMItem>(*containers[i]);
//}
