#include "BHTML.h"
// Json encode
void BHTML::JsonEncode(String * _s, int _position, String _key, String _val){	// header http + content
	switch (_position) {
        case BJSONONE:
        case BJSONFIRST:
		//	*_s += HTTP_header_ok;
		//	*_s += FPSTR(lb_HTTP_200);
            *_s += BJSON_OPEN_BRAKE;
            JsonAdd(_s,_key,_val);
            *_s+= (_position==BJSONONE) ? BJSON_CLOSE_BRAKE : BJSON_NEW_LINE;
            break;
        case BJSONNEXT:
            JsonAdd(_s,_key,_val);
            *_s+= BJSON_NEW_LINE;
            break;
        case BJSONLAST:
            JsonAdd(_s,_key,_val);
            *_s+= BJSON_CLOSE_BRAKE;
            break;
    };
}
String JsonDecode(String * _s, String _key){
    uint16_t startPosition = 0, endPosition = 0;
    startPosition = _s->indexOf(_key,startPosition);
    if(startPosition<0) return "";
    startPosition = _s->indexOf(':"',startPosition);
    endPosition = _s->indexOf('"',startPosition+2);
    return _s->substring(startPosition+2, endPosition);
}
// Add key, value to Json String
void BJSON::JsonAdd(String *_s, String _key,String _val) {
    *_s += BJSON_DOUBLE_QUOTE + _key + BJSON_DOUBLE_QUOTE + BJSON_DOUBLE_DOT + BJSON_DOUBLE_QUOTE + _val + BJSON_DOUBLE_QUOTE;
}