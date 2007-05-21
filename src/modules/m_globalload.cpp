/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2007 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

/* $ModDesc: Allows global loading of a module. */

#include "inspircd.h"
#include "users.h"
#include "channels.h"
#include "modules.h"

/** Handle /GLOADMODULE
 */
class cmd_gloadmodule : public command_t
{
 public:
	cmd_gloadmodule (InspIRCd* Instance) : command_t(Instance,"GLOADMODULE", 'o', 1)
	{
		this->source = "m_globalload.so";
		syntax = "<modulename>";
	}

	CmdResult Handle (const char** parameters, int pcnt, userrec *user)
	{
		if (ServerInstance->LoadModule(parameters[0]))
		{
			ServerInstance->WriteOpers("*** NEW MODULE '%s' GLOBALLY LOADED BY '%s'",parameters[0],user->nick);
			user->WriteServ("975 %s %s :Module successfully loaded.",user->nick, parameters[0]);

			/* route it! */
			return CMD_SUCCESS;
		}
		else
		{
			user->WriteServ("974 %s %s :Failed to load module: %s",user->nick, parameters[0],ServerInstance->ModuleError());
			/* XXX - returning CMD_FAILURE here could potentially mean half the net loads it, half doesn't. pass it on anyway? -- w00t */
			return CMD_FAILURE;
		}
	}
};

/** Handle /GUNLOADMODULE
 */
class cmd_gunloadmodule : public command_t
{
 public:
	cmd_gunloadmodule (InspIRCd* Instance) : command_t(Instance,"GUNLOADMODULE", 'o', 1)
	{
		this->source = "m_globalload.so";
		syntax = "<modulename>";
	}

	CmdResult Handle (const char** parameters, int pcnt, userrec *user)
	{
		if (ServerInstance->UnloadModule(parameters[0]))
		{
			ServerInstance->WriteOpers("*** MODULE '%s' GLOBALLY UNLOADED BY '%s'",parameters[0],user->nick);
			user->WriteServ("973 %s %s :Module successfully unloaded.",user->nick, parameters[0]);
			/* route it! */
			return CMD_SUCCESS;
		}
		else
		{
			/* XXX - see above note about returning CMD_FAILURE here -- w00t */
			user->WriteServ("972 %s %s :Failed to unload module: %s",user->nick, parameters[0],ServerInstance->ModuleError());
			return CMD_FAILURE;
		}
	}
};

/** Handle /GRELOADMODULE
 */
class cmd_greloadmodule : public command_t
{
 public:
	cmd_greloadmodule (InspIRCd* Instance) : command_t(Instance, "GRELOADMODULE", 'o', 1)
	{
		this->source = "m_globalload.so";
		syntax = "<modulename>";
	}

	CmdResult Handle(const char** parameters, int pcnt, userrec *user)
	{
		if (!ServerInstance->UnloadModule(parameters[0]))
		{
			user->WriteServ("972 %s %s :Failed to unload module: %s",user->nick, parameters[0],ServerInstance->ModuleError());
			return CMD_FAILURE;
		}

		if (!ServerInstance->LoadModule(parameters[0]))
		{
			user->WriteServ("974 %s %s :Failed to load module: %s",user->nick, parameters[0],ServerInstance->ModuleError());
			return CMD_FAILURE;
		}

		ServerInstance->WriteOpers("*** MODULE '%s' GLOBALLY RELOADED BY '%s'",parameters[0],user->nick);
		user->WriteServ("975 %s %s :Module successfully loaded.",user->nick, parameters[0]);
		
		return CMD_SUCCESS;
	}
};

class ModuleGlobalLoad : public Module
{
	cmd_gloadmodule *mycommand;
	cmd_gunloadmodule *mycommand2;
	cmd_greloadmodule *mycommand3;
	
 public:
	ModuleGlobalLoad(InspIRCd* Me) : Module(Me)
	{
		
		mycommand = new cmd_gloadmodule(ServerInstance);
		mycommand2 = new cmd_gunloadmodule(ServerInstance);
		mycommand3 = new cmd_greloadmodule(ServerInstance);
		ServerInstance->AddCommand(mycommand);
		ServerInstance->AddCommand(mycommand2);
		ServerInstance->AddCommand(mycommand3);
	}
	
	virtual ~ModuleGlobalLoad()
	{
	}
	
	virtual Version GetVersion()
	{
		return Version(1, 1, 0, 0, VF_VENDOR, API_VERSION);
	}
};


class ModuleGlobalLoadFactory : public ModuleFactory
{
 public:
	ModuleGlobalLoadFactory()
	{
	}
	
	~ModuleGlobalLoadFactory()
	{
	}
	
	virtual Module * CreateModule(InspIRCd* Me)
	{
		return new ModuleGlobalLoad(Me);
	}
	
};


extern "C" DllExport void * init_module( void )
{
	return new ModuleGlobalLoadFactory;
}
