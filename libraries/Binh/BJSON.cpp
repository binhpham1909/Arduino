#include "BJSON.h"
// Json encode
void BJSON::JsonEncode(int _position, String * _s, String _key, String _val){	// header http + content
	switch (_position) {
        case BJSONONE:
        case BJSONFIRST:
		//	*_s += HTTP_header_ok;
		//	*_s += FPSTR(lb_HTTP_200);
            *_s += FPSTR(lb_JSON_OPEN_BRAKE);
            JsonAdd(_s,_key,_val);
            *_s+= (_position==BJSONONE) ? FPSTR(lb_JSON_CLOSE_BRAKE) : FPSTR(lb_JSON_NEW_LINE);
            break;
        case BJSONNEXT:
            JsonAdd(_s,_key,_val);
            *_s+= FPSTR(lb_JSON_NEW_LINE);
            break;
        case BJSONLAST:
            JsonAdd(_s,_key,_val);
            *_s+= FPSTR(lb_JSON_CLOSE_BRAKE);
            break;
    };
}
// Add key, value to Json String
void BJSON::JsonAdd(String *_s, String _key,String _val) {
    *_s += '"' + _key + '"' + ":" + '"' + _val + '"';
}