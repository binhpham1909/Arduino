#include "JSON.h"
// Json encode
void JSON::Encode(int _position, String * _s, String _key, String _val){	// header http + content
	switch (_position) {
        case ONEJSON:
        case FIRSTJSON:
		//	*_s += HTTP_header_ok;
		//	*_s += FPSTR(lb_HTTP_200);
            *_s += FPSTR(lb_JSON_OPEN_BRAKE);
            Add(_s,_key,_val);
            *_s+= (_position==ONEJSON) ? FPSTR(lb_JSON_CLOSE_BRAKE) : FPSTR(lb_JSON_NEW_LINE);
            break;
        case NEXTJSON:
            Add(_s,_key,_val);
            *_s+= FPSTR(lb_JSON_NEW_LINE);
            break;
        case LASTJSON:
            Add(_s,_key,_val);
            *_s+= FPSTR(lb_JSON_CLOSE_BRAKE);
            break;
    };
}
// Add key, value to Json String
void JSON::Add(String *_s, String _key,String _val) {
    *_s += '"' + _key + '"' + ":" + '"' + _val + '"';
}