#include "Directory.h"
#include "Error.h"

#if defined __GNUC__

#include <unistd.h>
#include <linux/limits.h>

#elif defined  _WIN32

#include <Windows.h>

#endif


_JUNK_BEGIN

//! カレントディレクトリを取得する
ibool Directory::GetCurrent(
	std::string& curDir //<! [out] カレントディレクトリパス名が返る
) {
#if defined __GNUC__
	char buf[PATH_MAX];
	if (getcwd(buf, PATH_MAX) == NULL) {
		Error::SetLastErrorFromErrno();
		return false;
	}
	curDir = buf;
	return true;
#elif defined  _WIN32
	char buf[MAX_PATH];
	if (!::GetCurrentDirectoryA(sizeof(buf), buf)) {
		Error::SetLastErrorFromWinErr();
		return false;
	}
	curDir = buf;
	return true;
#endif
}

_JUNK_END
