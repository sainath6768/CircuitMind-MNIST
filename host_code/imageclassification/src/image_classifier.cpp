#include "image_classifier.h"
#include "sd_data_handler.h"
#include "serial_port_handler.h"
#include "mnist_onnx.h"

int ImageClassifier(char *portname) 
{
    list<string> directory_list;
    list<string> files_list;
	map<string,list<string>> directory_files;
    int decider;
    int* fd_ptr = PortnameChecker(portname); 

    if (fd_ptr == nullptr || *fd_ptr < 0) 
    {
        LOG(LOGGER_LEVEL_INFO, "BAD CONFIGURATION...BYE BYE");
        return 0;
    }

    int fd = *fd_ptr;  

    decider = ConfigureSerialPort(fd);
    if (!decider) 
    {
        LOG(LOGGER_LEVEL_INFO, "Successfully configured the serial port...");
    } else
    {
        LOG(LOGGER_LEVEL_INFO, "Serial port is not configured successfully...");
        return 0;
    }
    int model_up = OnnxInference(std::string("modelpath"),MODEL_PATH);
    while (true) 
    {
        
        std::string command;
        std::string response="";
        std::string homedir="./data/images/";
        LOG(LOGGER_LEVEL_INFO,"Enter a command (init, inference, ls, cat) or type 'exit' to quit: ");
        std::getline(std::cin, command);
        LOG(LOGGER_LEVEL_INFO,"Sending command %s ",command.c_str());
        if (command == "exit") 
        {
            break;
        }
        
        if(command == "init") 
        {

            writeToSerial(fd, command);
            response = readFromSerial(fd);
            LOG(LOGGER_LEVEL_INFO,"Response : %s",response.c_str());
            while(response == "") 
            {
                response = readFromSerial(fd);
                if (response == "NACK") 
                {

                    LOG(LOGGER_LEVEL_INFO, "NACK received. SD card is not initialized.");
                    return 0;
                }
            }
            
        }

         
        else if(command == "list_dir")
        {
            if(directory_list.empty())
            {
                writeToSerial(fd, command);
                response= ReadContiuFromSerial(fd);  
                ReadSdFiles(directory_list,response);
                string list_dir_cmd;
                for (const auto& directory : directory_list)
                {
                    list_dir_cmd="ls ";
                    list_dir_cmd += directory;
                    string full_path= homedir+directory;
                    mkdir(full_path.c_str(),0777);
                    LOG(LOGGER_LEVEL_INFO,"%s",list_dir_cmd.c_str());
                    writeToSerial(fd, list_dir_cmd);
                    response="";
                    response= ReadContiuFromSerial(fd);
                    list<string> file_list;
                    ReadSdFiles(file_list,response);
                    directory_files[directory]=file_list;
                    list_dir_cmd="";
                }
            }
            else if(!directory_list.empty())
            {
                // Printing the directory list
                // cout << "Directory list:" << endl;
                LOG(LOGGER_LEVEL_INFO,"Directory list:");
                for (const auto& directory : directory_list)
                {
                    // cout << directory << endl;
                    LOG(LOGGER_LEVEL_INFO,"%s",directory.c_str());
                   
                }
            }
           
        }
 
        else if( command == "show files")
        {
            DisplayFileContent(directory_files);
        }
        else if (command=="run")
        {
            readFromSerial(fd);
            string file_path="";
            command="";
            std::getline(std::cin, file_path);
            command = "cat "+file_path;
            writeToSerial(fd, command);
            response= DataReadContiuFromSerial(fd);	
            LOG(LOGGER_LEVEL_INFO,"response %s ",response.c_str());
            string resp_file= homedir+file_path;
            LOG(LOGGER_LEVEL_INFO,"respnse file path %s",resp_file.c_str());           
            int fd1=open(resp_file.c_str(),O_WRONLY|O_CREAT,0777);
            if(fd1<0)
            {
                LOG(LOGGER_LEVEL_INFO,"file opened failed");
                return 0;
            }
            unsigned char image_data[1024]={0};
            response=response.substr(0, response.size() - 3);
            memcpy(image_data,response.c_str(),response.size());
            LOG(LOGGER_LEVEL_INFO,"Size of response %ld ",size(response));
            LOG(LOGGER_LEVEL_INFO,"Response %s",response.c_str());
            int n = 0;
            n=  write(fd1,image_data,response.size());
            LOG(LOGGER_LEVEL_INFO,"%d ",n);
            
            if (model_up==0)
            {

                LOG(LOGGER_LEVEL_INFO,"Sucessfully model is build boss...");
                int is_pre_done =OnnxInference(std::string("imagepath"),resp_file);
            
                if(is_pre_done==0)
                {
                    LOG(LOGGER_LEVEL_INFO,"Sucessfully completed the prediction");
                }
                    
            }
            else if (model_up==1)
            {
            LOG(LOGGER_LEVEL_INFO,"unscessful to load the model");
            }
        }

    }
    directory_list.clear();
    return 0;  
}
