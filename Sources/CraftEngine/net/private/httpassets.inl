#pragma once

#include <unordered_map>
#include <string>

namespace CraftEngine
{
	namespace net
	{

	
		namespace _Private_Inline_Assets
		{

			static const std::unordered_map<std::string, std::string> _Http_File_Type_To_Content_Type_Map =
			{ 
				// Images
				{"png", "image/png"},
				{"bmp", "image/bmp"},
				{"gif", "image/gif"},
				{"jpeg", "image/jpeg"},
				{"jpg", "image/jpeg"},
				{"tif", "image/tiff"},
				{"tiff", "image/tiff"},
				{"xbm", "image/xbm"},
				{"ico", "image/x-icon"},

				{"wrl", "model/vrml"},
				{"htm", "text/html"},
				{"html", "text/html"},
				{"c", "text/plain"},
				{"cpp", "text/plain"},
				{"def", "text/plain"},
				{"h", "text/plain"},
				{"txt", "text/plain"},
				{"rtx", "text/richtext"},
				{"rtf", "text/richtext"},
				{"java", "text/x-java-source"},
				{"css", "text/css"},

				// Video
				{"mp4", "video/mpeg4"},
				{"mpeg", "video/mpeg"},
				{"mpg", "video/mpeg"},
				{"mpe", "video/mpeg"},
				{"avi", "video/msvideo"},
				{"mov", "video/quicktime"},
				{"qt", "video/quicktime"},

				// Audio
				{"aif", "audio/aiff"},
				{"aifc", "audio/aiff"},
				{"aiff", "audio/aiff"},
				{"au", "audio/basic"},
				{"snd", "audio/basic"},
				{"mid", "audio/midi"},
				{"rmi", "audio/midi"},
				{"mp3", "audio/mpeg"},
				{"vox", "audio/voxware"},
				{"wav", "audio/wav"},
				{"ra", "audio/x-pn-realaudio"},
				{"ram", "audio/x-pn-realaudio"},

				{"pdf", "application/pdf" },
				{"doc", "application/msword"},
				{"bin", "application/octet-stream"},
				{"dll", "application/octet-stream"},
				{"exe", "application/octet-stream"},
				{"p7c", "application/pkcs7-mime"},
				{"ai", "application/postscript"},
				{"eps", "application/postscript"},
				{"ps", "application/postscript"},
				{"rtf", "application/rtf"},
				{"fdf", "application/vnd.fdf"},
				{"arj", "application/x-arj"},
				{"rar", "application/x-compressed"},
				{"gz", "application/x-gzip"},
				{"class", "application/x-java-class"},
				{"js", "application/x-javascript"},
				{"lzh", "application/x-lzh"},
				{"lnk", "application/x-ms-shortcut"},
				{"tar", "application/x-tar"},
				{"hlp", "application/x-winhelp"},
				{"cert", "application/x-x509-ca-cert"},
				{"zip", "application/zip"},
				{"cab", "application/x-compressed"},
				{"arj", "application/x-compressed"},


				{"shtml", "wwwserver/html-ssi"},
				{"asa", "wwwserver/isapi"},
				{"asp", "wwwserver/isapi"},
				{"cfm", "wwwserver/isapi"},
				{"dbm", "wwwserver/isapi"},
				{"isa", "wwwserver/isapi"},
				{"plx", "wwwserver/isapi"},
				{"url", "wwwserver/isapi"},
				{"cgi", "wwwserver/isapi"},
				{"php", "wwwserver/isapi"},
				{"wcgi", "wwwserver/isapi"}
			};

			static const char* _Http_Default_Index_Page =
				"<!DOCTYPE html>"
				"<html>"
				"<head>"
				"<meta http - equiv = \"Content-Type\" content = \"text/html; charset=utf-8\"/>"
				"<title></title>"
				"<meta charset = \"utf-8\" / >"
				"</head>"
				"<body style = \"text-align:center;background-color: bisque; padding-top: 100px;\">"
				"<h5>hello world</h5>"
				"<select id = \"ab\">"
				"<option value = \"0\">html</option>"
				"<option value = \"1\">css</option>"
				"<option value = \"2\">js</option>"
				"</select>"
				"<div id = \"div1\" data - zg = \"data\" data - zgr = \"This is a div\">"
				"Hello World!"
				"</div>"
				"</body>"
				"</html>";
		}










	}
}

