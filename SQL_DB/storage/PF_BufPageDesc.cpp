#include "PF_BufPageDesc.h"


PF_BufPageDesc::PF_BufPageDesc() {
	pData = new char[Global_Paras::Block_Size];
}