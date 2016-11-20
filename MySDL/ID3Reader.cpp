#include "ID3Reader.h"



ID3Reader::ID3Reader()
{
}

int ID3Reader::openfile(const std::string & name)
{
	SDL_RWops* file = SDL_RWFromFile(name.c_str(), "rb");
	if (!file)
		return -1;
	if (parse_header(file) < 0)
		return parse_id3v1(file);
	if (parse_id3(file) < 0)
		return -1;
	return 0;
}

int ID3Reader::parse_header(SDL_RWops*file)
{
	char buf[20];
	Uint32 s = 0;

	memset(&vh, 0, sizeof(vh));
	SDL_RWread(file, buf, 20, 1);

	vh.id3[0] = buf[0];
	vh.id3[1] = buf[1];
	vh.id3[2] = buf[2];
	vh.id3[3] = 0;
	vh.ver_major = buf[3];
	vh.ver_minor = buf[4];
	vh.flags = buf[5];

	for (int i = 0; i < 4; i++)
	{
		s = s << 7;
		s |= buf[6 + i];

	}
	vh.size = s;

	if (vh.ver_major != 3)
	{
		DBGLOGEX(Error, "Can't parse id3 with major:%d (Only major:3)", vh.ver_major);
		return -1;
	}
	return 0;
}

int ID3Reader::get_frame_header(Uint8 *buf)
{
	int i;
	Uint32 b, s = 0;
	Uint16 f = 0;

	fh.id[0] = buf[0];
	fh.id[1] = buf[1];
	fh.id[2] = buf[2];
	fh.id[3] = buf[3];

	for (i = 0; i < 4; i++)
	{
		b = buf[4 + i];
		s = s << 8;
		s |= b;
	}
	fh.size = s;

	for (i = 0; i < 2; i++)
	{
		f = f << 8;
		f |= buf[8 + i];

	}
	fh.flags = f;
	return 0;
}

int ID3Reader::parse_id3(SDL_RWops*file)
{
	Uint8 *buf, *tmp;

	SDL_RWseek(file, 0, RW_SEEK_SET);
	tmp = new Uint8[vh.size + 10];
	SDL_RWread(file, tmp, vh.size + 10, 1);
	buf = tmp + 10;
	for (int i = 0; i < vh.size - 10;)
	{
		get_frame_header(buf + i);
		if (vh.size - i < fh.size)
		{
			DBGLOGEX(Error, "Error id3_v2_header size");
			free(tmp);
			return -1;
		}
		if (fh.size != 0)
		{
			add_id3tag(buf + i);
			i += fh.size + 10;
		}
	}
	return 0;
}

int ID3Reader::add_id3tag(Uint8 * buf)
{
	int i;
	id3_v2_frame_contents id3;
	memcpy(id3.id, fh.id, 4);
	id3.contents = new Uint8[fh.size];
	memcpy(id3.contents, buf + 11, fh.size - 1);
	id3.size = fh.size - 1;

	if (id3.size != 0)
	{
		if (strncmp((char*)id3.id, "APIC", 4) != 0 && strncmp((char*)id3.id, "TXXX", 4) != 0)
		{
			std::string out_str;
			wstou8s(out_str, (wchar_t*)id3.contents);
			delete id3.contents;
			id3.size = out_str.length() + 1;
			id3.contents = new Uint8[id3.size];
			strncpy((char*)id3.contents,out_str.c_str(), id3.size);

		}
	}
	tags.push_back(id3);
	return 0;
}

int ID3Reader::parse_id3v1(SDL_RWops * file)
{
	char buf[128];
	id3_v1_info*id3;
	SDL_RWseek(file, -128, RW_SEEK_END);
	int a=SDL_RWread(file,buf,128,1);
	id3 = (id3_v1_info*)buf;

	id3->Title[29] = 0;
	id3->Artist[29] = 0;
	id3->Album[29] = 0;
	id3->Comment[29] = 0;
	title = id3->Title;
	artist = id3->Artist;
	album = id3->Album;
	comment = id3->Comment;
	return 0;
}

int ID3Reader::getlength()
{
	return 0;
}


ID3Reader::~ID3Reader()
{
}
