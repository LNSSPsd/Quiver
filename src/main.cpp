#define cimg_display 0
#define cimg_verbosity 0
#define cimg_use_png
#include <iostream>
#include <argparser.h>
#include <json/json.h>
#include "../CImg/CImg.h"
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

using namespace cimg_library;

char *random_uuid( char buf[37] )
{
	const char *c = "89ab";
	char *p = buf;
	for(int n=0;n<16;n++){
		int b=rand()%255;
		switch(n){
			case 6:
				sprintf(p,"4%x",b%15 );
				break;
			case 8:
				sprintf(p,"%c%x",c[rand()%strlen( c )],b%15 );
				break;
			default:
				sprintf(p,"%02x",b );
				break;
		}
		p+=2;
		switch(n)
		{
			case 3:
			case 5:
			case 7:
			case 9:
				*p++ = '-';
				break;
		}
	}
	*p = 0;
	return buf;
}

int main(int argc,char *argv[]){
	argparser::arg_parser parser;
	argparser::arg<std::string> path(parser,"--path","-p","The path of the GIF or Videoc file");
	argparser::arg<std::string> rpname(parser,"--name","-n","Output resource pack's name");
	argparser::arg<std::string> rpdesc(parser,"--description","-d","Output resource pack's description");
	argparser::arg<float> width(parser,"--width","-w","The width of particle that will play your video",1.f);
	argparser::arg<float> height(parser,"--height","-h","The height of particle that will play your video",1.f);
	argparser::arg<std::string> outputFile(parser,"--output","-o","Output file path");
	argparser::arg<bool> showVersion(parser,"--version","-v","Prints version info");
	if(!parser.parse(argc,(const char**)argv)){
		return 1;
	}
	if(showVersion){
		std::cout<<"Quiver v1.0\nAuthor: Ruphane"<<std::endl;
		return 0;
	}
	if(path.get().empty()||rpname.get().empty()||rpdesc.get().empty()){
		std::cerr<<"Some of required arguments missing."<<std::endl;
		return 1;
	}
	
	if(outputFile.get().empty()){
		std::cerr<<"WARN: \"-o\" not specified,default to ./"<<rpname.get()<<std::endl;
		mkdir(rpname.get().c_str(),S_IRUSR | S_IWUSR | S_IXUSR);
		chdir(rpname.get().c_str());
	}else{
		mkdir(outputFile.get().c_str(),S_IRUSR | S_IWUSR | S_IXUSR);
		chdir(outputFile.get().c_str());
	}
	mkdir("res",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("res/textures",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("res/textures/frames",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("res/particles",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("res/particles/frames",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("beh",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("beh/functions",S_IRUSR | S_IWUSR | S_IXUSR);
	mkdir("beh/functions/frames",S_IRUSR | S_IWUSR | S_IXUSR);
	CImgList<unsigned char> img;
	try{
		img.load_video(path.get().c_str());
	}catch(std::exception err){
		try{
			img.load_gif_external(path.get().c_str());
		}catch(std::exception err2){
			std::cerr<<"The file specified is neither a video or a GIF image."<<std::endl;
			return 2;
		}
	}
	int imgsize=img.size();
	std::cout<<"Image loaded,frames="<<imgsize<<std::endl;
	int icount=0;
	std::cout<<"Extracting frames";
	char fn[64]={0};
	for(auto i:img){
		sprintf(fn,"res/textures/frames/deq_%d.png",icount);
		i.save_png(fn);
		icount++;
		printf("\033[M\rExtracting frames: [%d/%d]",icount,imgsize);
		fflush(stdout);
	}
	std::cout<<"\033[M\rSuccessfully extracted frames."<<std::endl;
	std::cout<<"Writing manifests...";
	fflush(stdout);
	char uuida[37]={0};
	char uuidb[37]={0};
	char uuidc[37]={0};
	char uuidd[37]={0};
	random_uuid(uuida);
	random_uuid(uuidb);
	random_uuid(uuidc);
	random_uuid(uuidd);
	Json::Value resRoot;
	Json::FastWriter fw;
	Json::Value versionInfo;
	versionInfo.append(Json::Value(1));
	versionInfo.append(Json::Value(0));
	versionInfo.append(Json::Value(0));
	Json::Value headerOfRes;
	headerOfRes["description"]=Json::Value(rpdesc.get());
	headerOfRes["name"]=Json::Value(rpname.get());
	headerOfRes["uuid"]=Json::Value(std::string(uuida));
	headerOfRes["version"]=versionInfo;
	resRoot["format_version"]=Json::Value(1);
	resRoot["header"]=headerOfRes;
	Json::Value moduleOfRes;
	moduleOfRes["description"]=Json::Value(rpdesc.get());
	moduleOfRes["type"]=Json::Value("resources");
	moduleOfRes["uuid"]=Json::Value(std::string(uuidb));
	moduleOfRes["version"]=versionInfo;
	Json::Value modulesOfRes;
	modulesOfRes.append(moduleOfRes);
	resRoot["modules"]=modulesOfRes;
	FILE *resManifest=fopen("res/manifest.json","wb");
	fprintf(resManifest,"%s",fw.write(resRoot).c_str());
	Json::Value behRoot;
	Json::Value headerOfBeh;
	headerOfBeh["description"]=Json::Value(rpdesc.get());
	headerOfBeh["name"]=Json::Value(rpname.get());
	headerOfBeh["uuid"]=Json::Value(std::string(uuidc));
	headerOfBeh["version"]=versionInfo;
	behRoot["format_version"]=Json::Value(1);
	behRoot["header"]=headerOfRes;
	Json::Value moduleOfBeh;
	moduleOfBeh["description"]=Json::Value(rpdesc.get());
	moduleOfBeh["type"]=Json::Value("data");
	moduleOfBeh["uuid"]=Json::Value(std::string(uuidd));
	moduleOfBeh["version"]=versionInfo;
	Json::Value modulesOfBeh;
	modulesOfBeh.append(moduleOfBeh);
	behRoot["modules"]=modulesOfBeh;
	Json::Value firstDependency;
	firstDependency["uuid"]=Json::Value(std::string(uuida));
	firstDependency["version"]=versionInfo;
	Json::Value dependencies;
	dependencies.append(firstDependency);
	behRoot["dependencies"]=dependencies;
	FILE *behManifest=fopen("beh/manifest.json","wb");
	fprintf(behManifest,"%s",fw.write(behRoot).c_str());
	fclose(resManifest);
	fclose(behManifest);
	std::cout<<"OK"<<std::endl;
	Json::Reader reader;
	Json::Value particle;
	reader.parse("{\"format_version\":\"1.10.0\",\"particle_effect\":{\"description\":{\"identifier\":\"\",\"basic_render_parameters\":{\"material\": \"particles_alpha\",\"texture\":\"\"}},\"components\": {\"minecraft:emitter_rate_instant\": {\"num_particles\": 1},\"minecraft:emitter_lifetime_once\": {\"active_time\": 0.05\"minecraft:emitter_shape_point\": {\"offset\":[0,0,0],\"direction\":[1,0,0]},\"minecraft:particle_lifetime_expression\": {\"max_lifetime\": 0.12},\"minecraft:particle_appearance_billboard\":{\"face_camera_mode\":\"lookat_xyz\",\"size\":[0,0]}}}}",particle);
	char identifier[128]={0};
	char idpath[128]={0};
	char fnx[64]={0};
	for(int i=0;i<imgsize;i++){
		sprintf(identifier,"%s:img_%d",rpname.get().c_str(),i);
		sprintf(idpath,"textures/frames/deq_%d.png",i);
		particle["particle_effect"]["description"]["identifier"]=Json::Value(std::string(identifier));
		particle["particle_effect"]["description"]["basic_render_parameters"]["texture"]=Json::Value(std::string(idpath));
		particle["particle_effect"]["components"]["minecraft:particle_appearance_billboard"]["size"][0]=Json::Value(width);
		particle["particle_effect"]["components"]["minecraft:particle_appearance_billboard"]["size"][1]=Json::Value(height);
		sprintf(fnx,"res/particles/frames/deq_%d.json",i);
		FILE *f=fopen(fnx,"wb");
		fprintf(f,"%s",fw.write(particle).c_str());
		fclose(f);
		printf("\033[M\rWriting particles for frames: [%d/%d]",i,imgsize);
		fflush(stdout);
	}
	std::cout<<"\033[M\rWrote particles."<<std::endl;
	std::cout<<"Writing function files...";
	fflush(stdout);
	FILE *loop=fopen("beh/functions/loop.mcfunction","wb");
	for(int i=0;i<imgsize;i++){
		fprintf(loop,"execute @a[scores={%s=%d}] ~ ~ ~ execute @e[type=armor_stand,name=%s] ~ ~ ~ particle %s:img_%d ~ ~ ~\n",rpname.get().c_str(),i,rpname.get().c_str(),rpname.get().c_str(),i);
	}
	fprintf(loop,"execute @p[scores={%s=..%d}] ~ ~ ~ scoreboard players add @s %s 1\n",rpname.get().c_str(),imgsize,rpname.get().c_str());
	fclose(loop);
	FILE *init=fopen("beh/functions/init.mcfunction","wb");
	fprintf(init,"scoreboard objectives remove %s\nscoreboard objectives add %s dummy %s\nscoreboard players add @p %s 0",rpname.get().c_str(),rpname.get().c_str(),rpname.get().c_str(),rpname.get().c_str());
	fclose(init);
	std::cout<<"OK\nEverything was done!"<<std::endl;
	return 0;
}
