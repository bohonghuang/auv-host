//
// Created by Qff on 2023/2/28.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <thread>
#include <map>
#include <functional>

namespace auv
{

class Application
{
public:
	static Application& GetInstance();

	void reload();
private:
	Application() = default;
	void run();
private:

};

}


#endif //APPLICATION_H
