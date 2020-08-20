#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <thread>
#include <algorithm>

#define DECRYPTION (int) Byte - (13*8^20/13)
#define CUSTOM_DECRYPTION (int) Byte + enc_key - (13*8^20/13)

#define ENCRYPTION (int) Byte + (13*8^20/13)
#define CUSTOM_ENCRYPTION (int) Byte - enc_key + (13*8^20/13)

struct fileInfo {
    std::string op;
    std::string src;
    std::string dest;
    std::string enc_k;
};

int getSize(const std::string);
void viewHelp();
void decrypt(void*);
void encrypt(void*);
bool exists(const std::string path);
bool num_only(const std::string &key);

int main(int argc, char *argv[]){
    std::string check;
    struct fileInfo file;

    if(argc != 5)
    {
        if(argc > 1)
        {
            file.op = argv[1];
            if(file.op == "-h")
            {
                viewHelp();
                return 0;
            }
            else
            {
                std::cout << "Invalid arguments, enter [-h] for help.\n";
                return 0;
            }
        }
        else
        {
            std::cout << "Invalid arguments, enter [-h] for help.\n";
            return 0;
        }

    }

    file.op = argv[1];
    file.src = argv[2];
    file.dest = argv[3];
    file.enc_k = argv[4];

    if(exists(file.src)==false)
    {
        std::cout << "The target file is not found.\n";
        return 0;
    }
    else if(num_only(file.enc_k)==false && file.enc_k != "-ud")
    {
        std::cout << "The custom encryption key cannot contain characters.\n";
        return 0;
    }
    else if(file.src == file.dest)
    {
        std::cout << "You cannot overwrite the target file, choose another save path.\n";
        return 0;
    }
    else if(exists(file.dest))
    {
        std::cout << "The output file [" << file.dest << "]" << " is already exists.\n";
        std::cout << "\nThis process will overwrite the old file. Proceed anyway (Y-N)? ";

        std::cin>>check;
        if(check[0]!='y' || check[0]!='Y')
            return 0;
    }
    if(file.op=="-e")
    {
        std::cout << "Target file : " << file.src;
        std::cout << "\nFile size : " << (double)getSize(file.src)/1048576 <<  " MB";
        std::cout << "\Flock is encrypting your file, please wait...\n\n";

        std::thread enc (encrypt, (void*) &file);
        enc.join();

        if(exists(file.dest))
        {
            std::cout << "\nProcess done!\n"
                 << "Your file saved as " << file.dest << std::endl;

            return 0;
        }
        else
        {
            std::cout << "\nAn error occurred while encrypting your file.\n";
            return 0;
        }
    }
    else if(file.op=="-d")
    {
        std::cout << "Target file : " << file.src;
        std::cout << "\nFile size : " << (double)getSize(file.src)/1048576 <<  " MB";
        std::cout << "\Flock is decrypting your file, please wait...\n\n";

        std::thread dec (decrypt, (void*) &file);
        dec.join();

        if(exists(file.dest))
        {
            std::cout << "\nProcess done!\n"
                 << "Your file saved as " << file.dest << std::endl;

            return 0;
        }
        else
        {
            std::cout << "\nAn error occurred while decrypting your file.\n";
            return 0;
        }
    }
    else
    {
        std::cout << "Invalid arguments, enter [-h] for help.\n";
        return 0;
    }


    return 0;

}
void viewHelp(){

    std::cout << "Usage : -command <input file> <output file> <enc_key>\n";
    std::cout << "Supported commands : \n\t\t-e\tFor encryption\n\t\t-d\tFor decryption\n\t\t-h\tView help\n";
    std::cout << "Encryption key : Numbers only. Or use -ud to use Flok's default encryption algorithm\n";
    std::cout << "Example : \n\t-e /home/username/imgs/pic.png /home/username/imgs/encrypted_pic.png 123456\n";
}

bool exists(const std::string path)
{
    std::ifstream f(path.c_str());
    return f.good();
}

bool num_only(const std::string &key)
{
    return std::all_of(key.begin(), key.end(),::isdigit);
}

int getSize(const std::string path)
{
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}


void encrypt(void *param)
{
    char Byte;
    struct fileInfo *arg = (struct fileInfo*) param;
    std::ifstream in;
    std::ofstream out;

    in.open(arg->src.c_str(), std::ios::in | std::ios::binary);
    out.open(arg->dest.c_str(), std::ios::out | std::ios::binary);

    int enc_key = atoi(arg->enc_k.c_str());

    int fileSize = getSize(arg->src.c_str());
    int current_size = 0;

    while(!in.eof())
    {
        char nByte;
        Byte = in.get();
        if(arg->enc_k=="-ud")
        {
            nByte = ENCRYPTION;
        }
        else
        {
            nByte = CUSTOM_ENCRYPTION;
        }
        out.put(nByte);
        ++current_size;
        double prog = ((double)current_size/(double)fileSize)*100;
        if(prog>100.00)
            prog=100.00;
        std::cout << '\r' << std::fixed << std::setprecision(2) << "Progress : " << prog << '%';
    }
    out.close();
    in.close();
}

void decrypt(void *param)
{
    char Byte;
    struct fileInfo *arg = (struct fileInfo*) param;
    std::ifstream in;
    std::ofstream out;

    in.open(arg->src.c_str(), std::ios::in | std::ios::binary);
    out.open(arg->dest.c_str(), std::ios::out | std::ios::binary);

    int enc_key = atoi(arg->enc_k.c_str());

    int fileSize = getSize(arg->src.c_str());
    int current_size = 0.0;

    while(!in.eof())
    {
        char nByte;
        Byte = in.get();
        if(arg->enc_k=="-ud")
        {
            nByte = DECRYPTION;
        }
        else
        {
            nByte = CUSTOM_DECRYPTION;
        }
        out.put(nByte);
        ++current_size;
        double prog = ((double)current_size/(double)fileSize)*100;
        if(prog>100.00)
            prog=100.00;
        std::cout << '\r' << std::fixed << std::setprecision(2) << "Progress : " << prog << '%';
    }
    in.close();
    out.close();
}
