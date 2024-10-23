#include "sd_data_handler.h"
#include "serial_port_handler.h"

void DisplayFileContent(map<string, list<string>> &mp) {
    for (const auto &dir : mp) {
        const string &directory = dir.first;
        const list<string> &files = dir.second;

        for (const auto &file : files) {
            string read_cmd = "cat " + directory + "/" + file;
			LOG(LOGGER_LEVEL_INFO,"READ CMD %s",read_cmd.c_str());
        }
    }
}

void ReadSdFiles(list<string> &list,string response)

{	
	
	stringstream ss(response);
	string item;
	list.clear();
	
	while(getline(ss,item,'\n'))
	{

		if((!strstr(item.c_str(),"Start" )) && (!strstr(item.c_str(),"End"))) {
 			transform(item.begin(), item.end(), item.begin(), ::tolower);
			list.push_back(item);
			LOG(LOGGER_LEVEL_INFO,"item %s ",item.c_str());
		}

	}
}

