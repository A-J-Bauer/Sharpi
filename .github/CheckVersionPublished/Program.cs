using CheckVersionPublished;
using System.Text;
using System.Text.Json;
using System.Xml;
using System.Xml.Linq;

// author: A.J.Bauer


/*  Parse.csproj file for Version and Title
    Get json versions array from NuGet
    Check if version found in csproj is in the array
 
    return < 0 on error
    return 0 if not found
    return 1 if found
*/



string[] cmdargs = Environment.GetCommandLineArgs();


if (cmdargs.Length != 2)
{
    Console.WriteLine("Bad number of arguments" + Environment.NewLine);
    Console.WriteLine($"Usage: {cmdargs[0]} <projectpath>");
    Environment.Exit(-1);
}

if (!cmdargs[1].EndsWith(".csproj"))
{
    Console.WriteLine($"File ending .csproj required" + Environment.NewLine);
    Console.WriteLine($"Usage: {cmdargs[0]} <projectpath>");
    Environment.Exit(-2);
}

string? githubActionPath = Environment.GetEnvironmentVariable("GITHUB_ACTION_PATH");

if (githubActionPath == null)
{
    Console.WriteLine($"Could not read GITHUB_ACTION_PATH environment variable" + Environment.NewLine);    
    Environment.Exit(-3);
}

string csprojFilePath = Path.Combine(githubActionPath, cmdargs[1]);

if (!File.Exists(csprojFilePath))
{
    Console.WriteLine($"File \"{cmdargs[1]}\" not found" + Environment.NewLine);
    Console.WriteLine($"Usage: {cmdargs[0]} <projectpath>");
    Environment.Exit(-4);
}

string csproj = "";

try
{
    csproj = File.ReadAllText(csprojFilePath, Encoding.UTF8);
}
catch (Exception ex)
{
    Console.WriteLine($"Error reading \"{csprojFilePath}\": {ex.Message}" + Environment.NewLine);    
    Environment.Exit(-5);
}

XDocument? xDocument = null;

try
{
    xDocument = XDocument.Parse(csproj);
}
catch (Exception ex)
{
    Console.WriteLine($"Error parsing \"{csprojFilePath}\": {ex.Message}" + Environment.NewLine);    
    Environment.Exit(-6);
}


XElement? title = xDocument?.Element("Project")?.Element("PropertyGroup")?.Element("Title");

if (title == null)
{
    Console.WriteLine($"<Project><PropertyGroup><Title> not found in \"{csprojFilePath}\"" + Environment.NewLine);    
    Environment.Exit(-7);
}

XElement? version = xDocument?.Element("Project")?.Element("PropertyGroup")?.Element("Version");

if (version == null)
{
    Console.WriteLine($"<Project><PropertyGroup><Version> not found in \"{csprojFilePath}\"" + Environment.NewLine);
    Environment.Exit(-8);
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
        Console.WriteLine($"Error retrieving versions array from NuGet" + Environment.NewLine);
        exitCode = -8;        
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
    Console.WriteLine($"Error deserializing versions array from NuGet" + Environment.NewLine);
    Environment.Exit(-9);
}

if (nugetVersions == null || nugetVersions.versions == null)
{
    Console.WriteLine($"Error deserializing versions array from NuGet" + Environment.NewLine);
    Environment.Exit(-9);
}

if (nugetVersions.versions.Contains(version.Value))
{
    Console.WriteLine($"CheckVersionPublished: Package {version.Value} already exists on Nuget.org");
    Environment.Exit(1);
}
else
{
    Console.WriteLine($"CheckVersionPublished: Package {version.Value} does not yet exist on Nuget.org");
    Environment.Exit(0);
}



