using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CheckVersionPublished
{
    /// <summary>
    /// Helper class for parsing json returned from
    /// https://api.nuget.org/v3-flatcontainer/Sharpi/index.json
    /// </summary>
    public class NugetVersions
    {        
        public string[]? versions { get; set; }
    }

}
