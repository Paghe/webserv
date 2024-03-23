#include "Response.hpp"

#define STATUSCODE _statusCode.getStati()

void Response::processPost()
{
	cout << "-----------------IN POST--------------------" << endl;
	cout << NORM << "URI: " << _URI << endl;
	_status = 201;
	string contentType = findKeyByValue(_configfile._mimeTypes, _request.getHeaders()["Content-Type"]);
	if (isCgi(_URI))
		{ formResponse(201, _statusInfo); return ; }
	if (isMultipart())
		{ formResponse(100, _statusInfo); return; }
	if (recieveQuery(contentType)) 
	{
		handleQuery();
		formResponse(201, "recieved Data");
		return ;
	}
	if (incorrectMimeType(_request.getHeaders()["Content-Type"]))
		{ formResponse(400, "Incorrect Mimetype"); }
	else if (!createFile(_URI, _request.getBody()))
		{ _status = 400; _statusInfo = "Please include a valid path. File could not be created"; }
	formResponse(_status, "");
}

bool Response::incorrectMimeType(const string & contentType)
{
	string filepath = _URI;
	if (!_request.getFilename().empty())
		filepath = _request.getFilename();
	size_t mimePos = filepath.find_last_of('.');
	string mime = "";
	if (mimePos != string::npos)
		mime = filepath.substr(mimePos + 1);
	if (mime.empty() && !(contentType == "text/plain" || contentType == "plain/text"))
		return true;
	if (_configfile._mimeTypes[mime] != contentType && !(contentType == "text/plain" || contentType == "plain/text"))
	{
		cout << _configfile._mimeTypes[mime] << endl;
		cout << contentType << endl;
		return true;
	}
	return false;
}

int Response::handleMultipart()
{
	formResponse(100, _statusInfo);
	if (Request::MultipartApproved == -1)
	{
		cout << "Multipart not Approved" << endl;
		formResponse(400, "Incorrect Boundary");
		Request::MultipartApproved = 0;
		return -1;
	}
	return 1;
}

int	Response::createFile(std::string & path, const std::string & content)
{
	fstream file;
	string filepath = path;
	if (!_request.getFilename().empty())
		filepath = _configfile._root + "/upload" + "/" + _request.getFilename();
	file.open(filepath.c_str(), ios::trunc | ios::binary | ios::out);
	if (!file.is_open())
	{
		cout << "File: " << filepath.c_str() << " could not be created." << endl;
		return false;
	}
	file << content;
	file.close();
	return true;
}

int	Response::isMultipart()
{
	if (!_request.boundary.empty())
	{
		cout << "isMultipart" << endl;
		return 1;
	}
	return 0;
}

bool Response::noFilename()
{
	if (!_request.getFilename().empty())
	{
		if (_request.getFilename() == ".empty.")
		{
			cout << "Multipart not approved, no filename" << endl;
			return 1;
		}
	}
	return 0;
}

std::string	Response::findKeyByValue(std::map<string, string>m, string value)
{
	std::string fileType;
	map<string, string>::iterator it;
	for (it = m.begin(); it != m.end(); it++)
	{
		if (it->second.compare(value) == 0)
		{
			fileType = it->first;
			break;
		}
	}
	return fileType;
}

bool			Response::recieveQuery(const string & contentType)
{
	if (contentType == "form-urlencoded")
	{
		_request.parseQuery(_request.getBody());
		Query query = _request.getQuery();
		Query::iterator it = query.begin();
		cout << "recieved? query: " << endl;
		for (; it != query.end(); it++)
		{
			cout << "query string:::::" << endl;
			cout << it->first << " " << it->second << endl;
		}
		return true;
	}
	return false;
	// if (query.empty())
	// 	return false;
	// return true;
}

void	Response::handleQuery()
{
	if (_request.getURI() != "/login")
		return ;
	bakeLoginCookie(_request.getQuery());
}

void	Response::bakeLoginCookie(Query & query)
{
	string session_id;
	cout << ORANGE << "I will create a cookie. YUMM" << endl;
	if (query.find("42login")!= query.end())
	{
		string login_name = query["42login"];
		if (!login_name.empty())
		{
			// session_id = "id=";
			session_id.append("2342342");  //generate id number
			if (_cookies.find(login_name) != _cookies.end())
				cout << RED << "user is already logged in. Wait 15 min until your session has expired" << endl;
			else
			{
				_cookies[login_name] = session_id;
				cout << MAG << "COOKIE was formed. still need to generate custom id" << NORM << endl;
			}
		}
	}
}