using CheckVersionPublished;
using System.Text;
using System.Text.Json;
using System.Xml;
using System.Xml.Linq;

// author: A.J.Bauer

/*  
    Input path to .csproj relative to repo root e.g. myproj/myproj.csproj

    Parse.csproj file for Version and Title
    Get json versions array from NuGet
    Check if version found in csproj is in the array
   
    if successful writes:
    
    check_version_published=yes
    check_version_published=no

    return 0     on success    
    return >0   failure
*/

string[] cmdargs = Environment.GetCommandLineArgs();


if (cmdargs.Length != 2)
{
    Console.WriteLine("Bad number of arguments\n");
    Console.WriteLine($"Usage: {cmdargs[0]} <projectpath>");
    Environment.Exit(1);
}

if (!cmdargs[1].EndsWith(".csproj"))
{
    Console.WriteLine($"File ending .csproj required\n");
    Console.WriteLine($"Usage: {cmdargs[0]} <projectpath>");
    Environment.Exit(2);
}

string? githubRunnerPath = Environment.GetEnvironmentVariable("GITHUB_WORKSPACE");

if (githubRunnerPath == null)
{
    Console.WriteLine($"Could not read GITHUB_WORKSPACE environment variable\n");    
    Environment.Exit(3);
}

string csprojFilePath = Path.Combine(githubRunnerPath, cmdargs[1]);

if (!File.Exists(csprojFilePath))
{
    Console.WriteLine($"File \"{cmdargs[1]}\" not found\n");
    Console.WriteLine($"Usage: {cmdargs[0]} <projectpath>");
    Environment.Exit(4);
}

string csproj = "";

try
{
    csproj = File.ReadAllText(csprojFilePath, Encoding.UTF8);
}
catch (Exception ex)
{
    Console.WriteLine($"Error reading \"{csprojFilePath}\": {ex.Message}\n");    
    Environment.Exit(5);
}

XDocument? xDocument = null;

try
{
    xDocument = XDocument.Parse(csproj);
}
catch (Exception ex)
{
    Console.WriteLine($"Error parsing \"{csprojFilePath}\": {ex.Message}\n");    
    Environment.Exit(6);
}


XElement? title = xDocument?.Element("Project")?.Element("PropertyGroup")?.Element("Title");

if (title == null)
{
    Console.WriteLine($"<Project><PropertyGroup><Title> not found in \"{csprojFilePath}\"\n");    
    Environment.Exit(7);
}

XElement? version = xDocument?.Element("Project")?.Element("PropertyGroup")?.Element("Version");

if (version == null)
{
    Console.WriteLine($"<Project><PropertyGroup><Version> not found in \"{csprojFilePath}\"\n");
    Environment.Exit(8);
}


string json = "";
int exitCode = 0;
using (HttpClient? httpClient = new HttpClient())
{    
    try
    {
        json = await httpClient.GetStringAsync($"https://api.nuget.org/v3-flatcontainer/{title.Value}/index.json");
    }
    catch (Exception)
    {
        Console.WriteLine($"Error retrieving versions array from NuGet\n");
        exitCode = 9;        
    }
}

if (exitCode != 0)
{
    Environment.Exit(exitCode);
}


NugetVersions? nugetVersions = null;
try
{
    nugetVersions = JsonSerializer.Deserialize<NugetVersions>(json);
}
catch (Exception)
{
    Console.WriteLine($"Error deserializing versions array from NuGet\n");
    Environment.Exit(10);
}

if (nugetVersions == null || nugetVersions.versions == null)
{
    Console.WriteLine($"Error deserializing versions array from NuGet\n");
    Environment.Exit(11);
}

if (nugetVersions.versions.Contains(version.Value))
{
    Console.WriteLine("check_version_published=yes");
    Console.WriteLine($"version={version.Value}");
    Environment.Exit(0);
}
else
{
    Console.WriteLine("check_version_published=no");
    Console.WriteLine($"version={version.Value}");
    Environment.Exit(0);
}



