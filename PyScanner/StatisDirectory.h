#pragma once

class CStatisDirectory
{
public:
	CStatisDirectory(void);
	~CStatisDirectory(void);

	static BOOL GetAppPath(OUT LPTSTR lpPath, IN size_t sBuffSize);
	static BOOL PathJoin(OUT LPTSTR lpPath, IN size_t sBuffSize, IN LPCTSTR lpDesPath, IN LPCTSTR lpSrcPath); //Æ´½ÓÂ·¾¶
};
