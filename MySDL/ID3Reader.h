#pragma once
#include "device.h"
#include <vector>
class ID3Reader
{
	static const int MAX_ID3_KEYS = 16;
	struct id3_v2_header
	{
		char id3[4];
		char ver_major;
		char ver_minor;
		char flags;
		uint32_t size;
	};

	struct id3_v2_frame_header
	{
		char id[4];
		uint32_t size;
		uint16_t flags;
	};

	struct id3_v2_frame_contents
	{
		unsigned char id[4];
		unsigned int size;
		unsigned char *contents;
	};

	struct id3_v1_info
	{
		char tag[3];
		char Title[30]; /*标题*/
		char Artist[30]; /*作者*/
		char Album[30]; /*专集*/
		char Year[4]; /*出品年代*/
		char Comment[30]; /*备注*/
		char Genre; /*类型*/
	};
public:
	id3_v2_header vh;
	id3_v2_frame_header fh;
	std::vector<id3_v2_frame_contents> tags;;

	std::string title;
	std::string artist;
	std::string album;
	std::string year;
	std::string comment;

	ID3Reader();
	int openfile(const std::string& name);
	int parse_header(SDL_RWops*file);
	int get_frame_header(Uint8 *buf);
	int parse_id3(SDL_RWops*file);
	int add_id3tag(Uint8*buf);
	int parse_id3v1(SDL_RWops *file);
	int getlength();
	~ID3Reader();
};

