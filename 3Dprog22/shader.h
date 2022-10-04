#ifndef SHADER_H
#define SHADER_H

/*Shadermanager has all ownership*/
struct Shader
{
    unsigned shaderId{0};
	
	[[nodiscard]]
	bool IsValid() const;
};

#endif
