#pragma once
#ifndef __JUNK_DIRECTORY_H__
#define __JUNK_DIRECTORY_H__

#include "JunkConfig.h"
#include <vector>
#include <string>

_JUNK_BEGIN

//! ディレクトリ
class Directory {
public:
	static ibool GetCurrent(std::string& curDir); //!< カレントディレクトリを取得する
};

_JUNK_END

#endif
