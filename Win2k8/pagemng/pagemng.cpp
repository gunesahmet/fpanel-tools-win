#include "pagemng.h"
using namespace System;
using namespace System::IO;

namespace Tools
{
	void pagemng::Main(array<String^> ^Args)
	{

		env->init();
		String ^parameters = String::Join(" ", Args);
		String ^currentFile = System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName;
		String ^str = currentFile + " " + parameters;
		logger->debugLog(str,999);


		if (utils->isAdmin() == false)
		{
			logger->errorLog("You have not right to access.", 1001);
			System::Environment::Exit(1001);
		}

		Arguments ^main_case = gcnew Arguments(Args);

		if (Args->Length == 0)
		{
			System::Environment::Exit(0);
		}

		String ^process = Args[0]->Replace("--", "");
		domain = main_case["domain"];
		prefix = main_case["prefix"];
		ipaddr = main_case["ipaddr"];
		user = main_case["user"];
		php = main_case["php"];
		python = main_case["python"];
		perl = main_case["perl"];
		asp = main_case["asp"];
		aspnet = main_case["aspnet"];

//C# TO C++ CONVERTER NOTE: The following 'switch' operated on a string variable and was converted to C++ 'if-else' logic:
//		switch(process)
//ORIGINAL LINE: case "init":
		if (process == "init")
		{
				init();
		}
//ORIGINAL LINE: case "set":
		else if (process == "set")
		{
				setDomain();
		}
		else
		{
				System::Environment::Exit(0);
		}
	}

	void pagemng::setDomain()
	{
		try
		{

			String ^php_button = "";
			String ^python_button = "";
			String ^perl_button = "";
			String ^asp_button = "";
			String ^aspnet_button = "";

			if (php == "on")
			{
				php_button = "<a class=\"btn btn-primary\" id=\"test_php\">Test PHP</a>";
				utils->copyFile(env->panel_core_dir + "\\templates\\default_pages\\test.php", env->vhosts_dir + prefix + "\\vhost\\httpdoc\\test.php");
			}
			if (python == "on")
			{
				python_button = "<a class=\"btn btn-primary\" id=\"test_python\">Test PYTHON</a>";
				utils->copyFile(env->panel_core_dir + "\\templates\\default_pages\\test.py", env->vhosts_dir + prefix + "\\vhost\\httpdoc\\test.py");
			}

			if (perl == "on")
			{
				perl_button = "<a class=\"btn btn-primary\" id=\"test_perl\">Test PERL</a>";
				utils->copyFile(env->panel_core_dir + "\\templates\\default_pages\\test.pl", env->vhosts_dir + prefix + "\\vhost\\httpdoc\\test.pl");
			}

			if (asp == "on")
			{
				asp_button = "<a class=\"btn btn-primary\" id=\"test_asp\">Test ASP</a>";
				utils->copyFile(env->panel_core_dir + "\\templates\\default_pages\\test.asp", env->vhosts_dir + prefix + "\\vhost\\httpdoc\\test.asp");
			}

			if (aspnet == "on")
			{
				aspnet_button = "<a class=\"btn btn-primary\" id=\"test_aspnet\">Test ASP.NET</a>";
				utils->copyFile(env->panel_core_dir + "\\templates\\default_pages\\test.aspx", env->vhosts_dir + prefix + "\\vhost\\httpdoc\\test.aspx");
			}

			String ^data = "<!DOCTYPE html>" + "\r\n" + "<html >" + "\r\n" + "	<head>" + "\r\n" + "		<meta charset=\"UTF-8\">" + "\r\n" + "		<title>FPanel</title>" + "\r\n" + "		<link rel=\"stylesheet\" href=\"style.css\">" + "\r\n" + "	</head>" + "\r\n" + "	<body>" + "\r\n" + "	<div class=\"warning-content\">" + "\r\n" + "		<h1>FPanel</h1>" + "\r\n" + "		<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\\" + "\r\n" + "			 width=\"100.001px\" height=\"70px\" viewBox=\"0 0 100 68\">" + "\r\n" + "		<g id=\"large\">" + "\r\n" + "			<g>" + "\r\n" + "			<path d=\"M55.777,38.473l6.221-1.133c0.017-1.791-0.123-3.573-0.41-5.324l-6.321-0.19c-0.438-2.053-1.135-4.048-2.076-5.931" + "\r\n" + "				l4.82-4.094c-0.868-1.552-1.874-3.028-3.005-4.417l-5.569,2.999c-1.385-1.54-2.98-2.921-4.771-4.099l2.124-5.954" + "\r\n" + "				c-0.759-0.452-1.543-0.878-2.357-1.269c-0.811-0.39-1.625-0.732-2.449-1.046l-3.325,5.381c-2.038-0.665-4.113-1.052-6.183-1.174" + "\r\n" + "				L31.34,6.002c-1.792-0.02-3.571,0.119-5.32,0.406l-0.191,6.32c-2.056,0.439-4.051,1.137-5.936,2.08l-4.097-4.82" + "\r\n" + "				c-1.546,0.872-3.022,1.875-4.407,3.006l2.996,5.566c-1.54,1.384-2.925,2.985-4.104,4.778c-2.16-0.771-4.196-1.498-5.953-2.127" + "\r\n" + "				c-0.449,0.765-0.875,1.544-1.265,2.354c-0.39,0.811-0.733,1.63-1.049,2.457c1.587,0.981,3.424,2.119,5.377,3.325" + "\r\n" + "				c-0.662,2.037-1.049,4.117-1.172,6.186l-6.218,1.136c-0.021,1.789,0.12,3.566,0.407,5.321l6.32,0.188" + "\r\n" + "				c0.442,2.06,1.143,4.057,2.082,5.937l-4.818,4.095c0.872,1.549,1.873,3.026,3.009,4.412l5.563-2.998" + "\r\n" + "				c1.392,1.54,2.989,2.92,4.777,4.099l-2.121,5.954c0.756,0.446,1.538,0.871,2.348,1.258c0.813,0.394,1.633,0.739,2.462,1.05" + "\r\n" + "				l3.326-5.375c2.033,0.662,4.109,1.05,6.175,1.17l1.137,6.221c1.791,0.019,3.569-0.123,5.323-0.407l0.194-6.324" + "\r\n" + "				c2.053-0.438,4.045-1.136,5.927-2.079l4.093,4.817c1.55-0.865,3.026-1.87,4.414-2.999l-2.995-5.572" + "\r\n" + "				c1.537-1.385,2.914-2.98,4.093-4.772l5.953,2.127c0.448-0.761,0.878-1.545,1.268-2.356c0.388-0.808,0.729-1.631,1.047-2.458" + "\r\n" + "				l-5.378-3.324C55.268,42.615,55.655,40.542,55.777,38.473z M42.302,42.435c-3.002,6.243-10.495,8.872-16.737,5.866" + "\r\n" + "				c-6.244-2.999-8.872-10.493-5.867-16.736c3.002-6.244,10.495-8.873,16.736-5.869C42.676,28.698,45.306,36.19,42.302,42.435z\"  fill=\"none\" stroke=\"#E43\"/>    <animateTransform" + "\r\n" + "					attributeName=\"transform\\" + "\r\n" + "					begin=\"0s\\" + "\r\n" + "					dur=\"3s\\" + "\r\n" + "					type=\"rotate\\" + "\r\n" + "					from=\"0 31 37\\" + "\r\n" + "					to=\"360 31 37\\" + "\r\n" + "					repeatCount=\"indefinite\\" + "\r\n" + "			</animateTransform>" + "\r\n" + "		</g>" + "\r\n" + "		<g id=\"small\">" + "\r\n" + "			<path d=\"M93.068,19.253L99,16.31c-0.371-1.651-0.934-3.257-1.679-4.776l-6.472,1.404c-0.902-1.436-2.051-2.735-3.42-3.819" + "\r\n" + "				l2.115-6.273c-0.706-0.448-1.443-0.867-2.213-1.238c-0.774-0.371-1.559-0.685-2.351-0.958l-3.584,5.567" + "\r\n" + "				c-1.701-0.39-3.432-0.479-5.118-0.284L73.335,0c-1.652,0.367-3.256,0.931-4.776,1.672l1.404,6.47" + "\r\n" + "				c-1.439,0.899-2.744,2.047-3.835,3.419c-2.208-0.746-4.38-1.476-6.273-2.114c-0.451,0.71-0.874,1.448-1.244,2.229" + "\r\n" + "				c-0.371,0.764-0.68,1.541-0.954,2.329c1.681,1.078,3.612,2.323,5.569,3.579c-0.399,1.711-0.486,3.449-0.291,5.145" + "\r\n" + "				c-2.086,1.034-4.143,2.055-5.936,2.945c0.368,1.648,0.929,3.25,1.67,4.769c1.954-0.426,4.193-0.912,6.468-1.405" + "\r\n" + "				c0.906,1.449,2.06,2.758,3.442,3.853l-2.117,6.27c0.708,0.449,1.439,0.865,2.218,1.236c0.767,0.371,1.551,0.685,2.338,0.96" + "\r\n" + "				c1.081-1.68,2.319-3.612,3.583-5.574c1.714,0.401,3.457,0.484,5.156,0.288L82.695,42c1.651-0.371,3.252-0.931,4.773-1.676" + "\r\n" + "				c-0.425-1.952-0.912-4.194-1.404-6.473c1.439-0.902,2.744-2.057,3.835-3.436l6.273,2.11c0.444-0.7,0.856-1.43,1.225-2.197" + "\r\n" + "				c0.372-0.777,0.691-1.569,0.963-2.361l-5.568-3.586C93.181,22.677,93.269,20.939,93.068,19.253z M84.365,24.062" + "\r\n" + "				c-1.693,3.513-5.908,4.991-9.418,3.302c-3.513-1.689-4.99-5.906-3.301-9.419c1.688-3.513,5.906-4.991,9.417-3.302" + "\r\n" + "				C84.573,16.331,86.05,20.549,84.365,24.062z\" fill=\"none\" stroke=\"#E43\"/>" + "\r\n" + "		  <animateTransform" + "\r\n" + "					attributeName=\"transform\\" + "\r\n" + "					begin=\"0s\\" + "\r\n" + "					dur=\"2s\\" + "\r\n" + "					type=\"rotate\\" + "\r\n" + "					from=\"0 78 21\\" + "\r\n" + "					to=\"-360 78 21\\" + "\r\n" + "					repeatCount=\"indefinite\\" + "\r\n" + "			</animateTransform>" + "\r\n" + "		</g>" + "\r\n" + "		</svg>" + "\r\n" + "		<p>" + "\r\n" + "		  This page automatically created by FPanel. <br />" + "\r\n" + "		  If you think there is a problem, please contact with system administrators." + "\r\n" + "		</p>" + "\r\n" + "\r\n" + "\r\n" + "		</div>" + "\r\n" + "\r\n" + "	</body>" + "\r\n" + "</html>";

			utils->writeFile(env->vhosts_dir + prefix + "\\vhost\\httpdoc\\index.html", data);
		}
		catch (Exception ^e)
		{
			logger->errorLog("pagemng::setDomain : " + e->Message, 1801);
		}
		System::Environment::Exit(0);

	}

	void pagemng::init()
	{
		try
		{
			String ^php_button = "<a class=\"btn btn-primary\" id=\"test_php\">Test PHP</a>";
			String ^perl_button = "<a class=\"btn btn-primary\" id=\"test_perl\">Test PERL</a>";
			String ^python_button = "<a class=\"btn btn-primary\" id=\"test_python\">Test PYTHON</a>";
			String ^asp_button = "<a class=\"btn btn-primary\" id=\"test_asp\">Test ASP</a>";
			String ^aspnet_button = "<a class=\"btn btn-primary\" id=\"test_aspnet\">Test ASP.NET</a>";

			utils->copyFile(env->panel_core_dir + "templates\\default_pages\\test.php", env->vhosts_dir + "default\\test.php");
			utils->copyFile(env->panel_core_dir + "templates\\default_pages\\test.py", env->vhosts_dir + "default\\test.py");
			utils->copyFile(env->panel_core_dir + "templates\\default_pages\\test.pl", env->vhosts_dir + "default\\test.pl");
			utils->copyFile(env->panel_core_dir + "templates\\default_pages\\test.asp", env->vhosts_dir + "default\\test.asp");
			utils->copyFile(env->panel_core_dir + "templates\\default_pages\\test.aspx", env->vhosts_dir + "default\\test.aspx");

			String ^data = "<!DOCTYPE html>" + "\r\n" + "<html >" + "\r\n" + "	<head>" + "\r\n" + "		<meta charset=\"UTF-8\">" + "\r\n" + "		<title>FPanel</title>" + "\r\n" + "		<link rel=\"stylesheet\" href=\"style.css\">" + "\r\n" + "	</head>" + "\r\n" + "	<body>" + "\r\n" + "	<div class=\"warning-content\">" + "\r\n" + "		<h1>FPanel</h1>" + "\r\n" + "		<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\\" + "\r\n" + "			 width=\"100.001px\" height=\"70px\" viewBox=\"0 0 100 68\">" + "\r\n" + "		<g id=\"large\">" + "\r\n" + "			<g>" + "\r\n" + "			<path d=\"M55.777,38.473l6.221-1.133c0.017-1.791-0.123-3.573-0.41-5.324l-6.321-0.19c-0.438-2.053-1.135-4.048-2.076-5.931" + "\r\n" + "				l4.82-4.094c-0.868-1.552-1.874-3.028-3.005-4.417l-5.569,2.999c-1.385-1.54-2.98-2.921-4.771-4.099l2.124-5.954" + "\r\n" + "				c-0.759-0.452-1.543-0.878-2.357-1.269c-0.811-0.39-1.625-0.732-2.449-1.046l-3.325,5.381c-2.038-0.665-4.113-1.052-6.183-1.174" + "\r\n" + "				L31.34,6.002c-1.792-0.02-3.571,0.119-5.32,0.406l-0.191,6.32c-2.056,0.439-4.051,1.137-5.936,2.08l-4.097-4.82" + "\r\n" + "				c-1.546,0.872-3.022,1.875-4.407,3.006l2.996,5.566c-1.54,1.384-2.925,2.985-4.104,4.778c-2.16-0.771-4.196-1.498-5.953-2.127" + "\r\n" + "				c-0.449,0.765-0.875,1.544-1.265,2.354c-0.39,0.811-0.733,1.63-1.049,2.457c1.587,0.981,3.424,2.119,5.377,3.325" + "\r\n" + "				c-0.662,2.037-1.049,4.117-1.172,6.186l-6.218,1.136c-0.021,1.789,0.12,3.566,0.407,5.321l6.32,0.188" + "\r\n" + "				c0.442,2.06,1.143,4.057,2.082,5.937l-4.818,4.095c0.872,1.549,1.873,3.026,3.009,4.412l5.563-2.998" + "\r\n" + "				c1.392,1.54,2.989,2.92,4.777,4.099l-2.121,5.954c0.756,0.446,1.538,0.871,2.348,1.258c0.813,0.394,1.633,0.739,2.462,1.05" + "\r\n" + "				l3.326-5.375c2.033,0.662,4.109,1.05,6.175,1.17l1.137,6.221c1.791,0.019,3.569-0.123,5.323-0.407l0.194-6.324" + "\r\n" + "				c2.053-0.438,4.045-1.136,5.927-2.079l4.093,4.817c1.55-0.865,3.026-1.87,4.414-2.999l-2.995-5.572" + "\r\n" + "				c1.537-1.385,2.914-2.98,4.093-4.772l5.953,2.127c0.448-0.761,0.878-1.545,1.268-2.356c0.388-0.808,0.729-1.631,1.047-2.458" + "\r\n" + "				l-5.378-3.324C55.268,42.615,55.655,40.542,55.777,38.473z M42.302,42.435c-3.002,6.243-10.495,8.872-16.737,5.866" + "\r\n" + "				c-6.244-2.999-8.872-10.493-5.867-16.736c3.002-6.244,10.495-8.873,16.736-5.869C42.676,28.698,45.306,36.19,42.302,42.435z\"  fill=\"none\" stroke=\"#E43\"/>    <animateTransform" + "\r\n" + "					attributeName=\"transform\\" + "\r\n" + "					begin=\"0s\\" + "\r\n" + "					dur=\"3s\\" + "\r\n" + "					type=\"rotate\\" + "\r\n" + "					from=\"0 31 37\\" + "\r\n" + "					to=\"360 31 37\\" + "\r\n" + "					repeatCount=\"indefinite\\" + "\r\n" + "			</animateTransform>" + "\r\n" + "		</g>" + "\r\n" + "		<g id=\"small\">" + "\r\n" + "			<path d=\"M93.068,19.253L99,16.31c-0.371-1.651-0.934-3.257-1.679-4.776l-6.472,1.404c-0.902-1.436-2.051-2.735-3.42-3.819" + "\r\n" + "				l2.115-6.273c-0.706-0.448-1.443-0.867-2.213-1.238c-0.774-0.371-1.559-0.685-2.351-0.958l-3.584,5.567" + "\r\n" + "				c-1.701-0.39-3.432-0.479-5.118-0.284L73.335,0c-1.652,0.367-3.256,0.931-4.776,1.672l1.404,6.47" + "\r\n" + "				c-1.439,0.899-2.744,2.047-3.835,3.419c-2.208-0.746-4.38-1.476-6.273-2.114c-0.451,0.71-0.874,1.448-1.244,2.229" + "\r\n" + "				c-0.371,0.764-0.68,1.541-0.954,2.329c1.681,1.078,3.612,2.323,5.569,3.579c-0.399,1.711-0.486,3.449-0.291,5.145" + "\r\n" + "				c-2.086,1.034-4.143,2.055-5.936,2.945c0.368,1.648,0.929,3.25,1.67,4.769c1.954-0.426,4.193-0.912,6.468-1.405" + "\r\n" + "				c0.906,1.449,2.06,2.758,3.442,3.853l-2.117,6.27c0.708,0.449,1.439,0.865,2.218,1.236c0.767,0.371,1.551,0.685,2.338,0.96" + "\r\n" + "				c1.081-1.68,2.319-3.612,3.583-5.574c1.714,0.401,3.457,0.484,5.156,0.288L82.695,42c1.651-0.371,3.252-0.931,4.773-1.676" + "\r\n" + "				c-0.425-1.952-0.912-4.194-1.404-6.473c1.439-0.902,2.744-2.057,3.835-3.436l6.273,2.11c0.444-0.7,0.856-1.43,1.225-2.197" + "\r\n" + "				c0.372-0.777,0.691-1.569,0.963-2.361l-5.568-3.586C93.181,22.677,93.269,20.939,93.068,19.253z M84.365,24.062" + "\r\n" + "				c-1.693,3.513-5.908,4.991-9.418,3.302c-3.513-1.689-4.99-5.906-3.301-9.419c1.688-3.513,5.906-4.991,9.417-3.302" + "\r\n" + "				C84.573,16.331,86.05,20.549,84.365,24.062z\" fill=\"none\" stroke=\"#E43\"/>" + "\r\n" + "		  <animateTransform" + "\r\n" + "					attributeName=\"transform\\" + "\r\n" + "					begin=\"0s\\" + "\r\n" + "					dur=\"2s\\" + "\r\n" + "					type=\"rotate\\" + "\r\n" + "					from=\"0 78 21\\" + "\r\n" + "					to=\"-360 78 21\\" + "\r\n" + "					repeatCount=\"indefinite\\" + "\r\n" + "			</animateTransform>" + "\r\n" + "		</g>" + "\r\n" + "		</svg>" + "\r\n" + "		<p>" + "\r\n" + "		  This page automatically created by FPanel. <br />" + "\r\n" + "		  If you think there is a problem, please contact with system administrators." + "\r\n" + "		</p>" + "\r\n" + "\r\n" + "\r\n" + "		</div>" + "\r\n" + "\r\n" + "	</body>" + "\r\n" + "</html>";

			utils->writeFile(env->vhosts_dir + "\\default\\index.html", data);
		}
		catch (Exception ^e)
		{
			logger->debugLog("pagemng: " + e->Message, 1802);
		}


	}
}

using namespace Tools;
int main(array<System::String ^> ^Args)
{
	pagemng ^mainProcess = gcnew pagemng();
	mainProcess->Main(Args);
	return 0;
}