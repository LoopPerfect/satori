#ifndef SATORI_STATUS_HPP
#define SATORI_STATUS_HPP

#include <string>

namespace satori {

constexpr char const* code2Msg(unsigned short const status) {
  switch (status) {
    case 100: return "100 Continue\r\n"; // [RFC7231, Section 6.2.1]
    case 101: return "101 Switching Protocols\r\n"; //  [RFC7231, Section 6.2.2]
    case 102: return "102 Processing\r\n"; //  [RFC2518]
    
    case 200: return "200 OK\r\n"; //[RFC7231, Section 6.3.1]
    case 201: return "201 Created\r\n"; //[RFC7231, Section 6.3.2]
    case 202: return "202 Accepted\r\n"; //[RFC7231, Section 6.3.3]
    case 203: return "203 No-Authoritative Information\r\n"; //  [RFC7231, Section 6.3.4]
    case 204: return "204 No Content\r\n"; //[RFC7231, Section 6.3.5]
    case 205: return "205 Reset Content\r\n";  //[RFC7231, Section 6.3.6]
    case 206: return "206 Partial Content\r\n"; //  [RFC7233, Section 4.1]
    case 207: return "207 Mult-Status\r\n"; // [RFC4918]
    case 208: return "208 Already Reported\r\n"; // [RFC5842]
    
    case 226: return "226 IM Used\r\n"; //  [RFC3229]
    
    case 300: return "300 Multiple Choices\r\n"; // [RFC7231, Section 6.4.1]
    case 301: return "301 Moved Permanently\r\n"; // [RFC7231, Section 6.4.2]
    case 302: return "302 Found\r\n"; // [RFC7231, Section 6.4.3]
    case 303: return "303 See Other\r\n"; // [RFC7231, Section 6.4.4]
    case 304: return "304 Not Modified\r\n"; //[RFC7232, Section 4.1]
    case 305: return "305 Use Proxy\r\n"; //  [RFC7231, Section 6.4.5]
    case 306: return "306 Unused\r\n"; // [RFC7231, Section 6.4.6]
    case 307: return "307 Temporary Redirect\r\n"; //[RFC7231, Section 6.4.7]
    case 308: return "308 Permanent Redirect\r\n"; // [RFC7538]
    
    case 400: return "400 Bad Request\r\n"; //  [RFC7231, Section 6.5.1]
    case 401: return "401 Unauthorized\r\n"; //[RFC7235, Section 3.1]
    case 402: return "402 Payment Required\r\n"; //[RFC7231, Section 6.5.2]
    case 403: return "403 Forbidden\r\n"; //[RFC7231, Section 6.5.3]
    case 404: return "404 Not Found\r\n"; //[RFC7231, Section 6.5.4]
    case 405: return "405 Method Not Allowed\r\n"; // [RFC7231, Section 6.5.5]
    case 406: return "406 Not Acceptable\r\n"; // [RFC7231, Section 6.5.6]
    case 407: return "407 Proxy Authentication Required\r\n"; //  [RFC7235, Section 3.2]
    case 408: return "408 Request Timeout\r\n"; //  [RFC7231, Section 6.5.7]
    case 409: return "409 Conflict\r\n"; //[RFC7231, Section 6.5.8]
    case 410: return "410 Gone\r\n"; //[RFC7231, Section 6.5.9]
    case 411: return "411 Length Required\r\n"; //  [RFC7231, Section 6.5.10]
    case 412: return "412 Precondition Failed\r\n"; //  [RFC7232, Section 4.2][RFC8144, Section 3.2]
    case 413: return "413 Payload Too Large\r\n"; //  [RFC7231, Section 6.5.11]
    case 414: return "414 URI Too Long\r\n"; // [RFC7231, Section 6.5.12]
    case 415: return "415 Unsupported Media Type\r\n"; // [RFC7231, Section 6.5.13][RFC7694, Section 3]
    case 416: return "416 Range Not Satisfiable\r\n"; // [RFC7233, Section 4.4]
    case 417: return "417 Expectation Failed\r\n"; // [RFC7231, Section 6.5.14]
   
    case 421: return "421 Misdirected Request\r\n"; //  [RFC7540, Section 9.1.2]
    case 422: return "422 Unprocessable Entity\r\n"; // [RFC4918]
    case 423: return "423 Locked\r\n"; //[RFC4918]
    case 424: return "424 Failed Dependency\r\n"; // [RFC4918]
    case 426: return "426 Upgrade Required\r\n"; // [RFC7231, Section 6.5.15]
    case 428: return "428 Precondition Required\r\n"; //  [RFC6585]
    case 429: return "429 Too Many Requests\r\n"; //  [RFC6585]
    
    case 431: return "431 Request Header Fields Too Large\r\n"; //  [RFC6585]
    case 451: return "451 Unavailable For Legal Reasons\r\n"; //  [RFC7725]
    case 500: return "500 Internal Server Error\r\n"; //  [RFC7231, Section 6.6.1]
    case 501: return "501 Not Implemented\r\n"; //  [RFC7231, Section 6.6.2]
    case 502: return "502 Bad Gateway\r\n"; //  [RFC7231, Section 6.6.3]
    case 503: return "503 Service Unavailable\r\n"; //  [RFC7231, Section 6.6.4]
    case 504: return "504 Gateway Timeout\r\n"; //  [RFC7231, Section 6.6.5]
    case 505: return "505 HTTP Version Not Supported\r\n"; // [RFC7231, Section 6.6.6]
    case 506: return "506 Variant Also Negotiates\r\n"; // [RFC2295]
    case 507: return "507 Insufficient Storage\r\n"; // [RFC4918]
    case 508: return "508 Loop Detected\r\n"; // [RFC5842]
    case 510: return "510 Not Extended\r\n"; // [RFC2774]
    case 511: return "511 Network Authentication Required\r\n"; //  [RFC6585]
    
    /*
    103-199 Unassigned  
    209-225 Unassigned  
    227-299 Unassigned  
    309-399 Unassigned  
    418-420 Unassigned 
    425 Unassigned
    427 Unassigned 
    430 Unassigned  
    432-450  Unassigned  
    452-499 Unassigned  
    509 Unassigned  
    512-599 Unassigned
    */

    default:
      return "";
  }


}

}

#endif
