#!/usr/bin/python

import json
import sys
import re

g_shader_sources = {}
g_programs = {}
g_deps = []

g_uniforms = set()
g_uniformdefs = set()
g_samplers = set()

g_directory = None
g_root_dir = "./"

class UniformDefs():
    def __init__(self):
        self.uniforms = []
        self.index = 0

    def __hash__(self):
        return reduce(lambda x,y: x + hash(y), self.uniforms, 0)

    def __repr__(self):
        return repr(self.uniforms)

    def __eq__(self,other):
        if len(self.uniforms) != len(other.uniforms): return False

        for i in range(0, len(self.uniforms)):
                if self.uniforms[i] != other.uniforms[i]: return False

        return True

class SamplerDefs():
    def __init__(self):
        self.samplers = []
        self.index = 0

    def __hash__(self):
        return reduce(lambda x,y: x + hash("%s=%d" % (y["name"],y["value"])), self.samplers, 0)

    def __repr__(self):
        return repr(self.samplers)

    def __eq__(self,other):
        if len(self.samplers) != len(other.samplers): return False

        for i in range(0, len(self.samplers)):
                if self.samplers[i] != other.samplers[i]: return False

        return True

def main():
    global g_directory
    global g_root_dir
    if len(sys.argv) > 1:
        g_root_dir = sys.argv[1]

    g_directory = g_root_dir + "data/shaders/"

    print "Building shaders from %s" % (g_directory)

    # TODO: check trailing slash

    parse()

    write_output()

def parse():
    global g_deps
    f = open(g_directory + "shaders.conf")
    g_deps.append(g_directory + "shaders.conf")
    # TODO: error handling
    conf = json.load(f)
    f.close()

    for program_name,sources in conf.iteritems():
        build_program(program_name, sources)

def build_program(name, sources):
    global g_programs
    print "Shader: %s" % name
    source_ids = g_programs[name] = []
    for source in sources:
        source_ids.append(build_source_def(source))

def build_source_def(filename):
    global g_shader_sources
    global g_uniforms
    global g_uniformdefs
    global g_samplers
    global g_deps

    if filename not in g_shader_sources:
        index = len(g_shader_sources)
        shader = g_shader_sources[filename] = {}
        shader["index"] = index

        f = open(g_directory + filename)
        g_deps.append(g_directory + filename)
        # TODO: error handling
        source = f.read()
        f.close()

        shader["source"] = source
        shader["stage"] = None

        shader["enum"] = re.sub("[^A-Za-z]+", "_", filename)

        udef = shader["uniforms"] = UniformDefs()
        sdef = shader["samplers"] = SamplerDefs()

        uniforms = {}

        # Find uniforms:
        uniform_decls = re.findall('[^;]\s*uniform\s+(.+?);', source, re.MULTILINE)
        for udecl in uniform_decls:
            split = re.split('[\s=;]', udecl.strip())
            utype = split[0].strip()
            name = split[1].strip()
            g_uniforms.add(name)
            uniforms[name] = utype
            if utype != "sampler2D":
                udef.uniforms.append(name)

        g_uniformdefs.add(udef)

        #Handle pragmas
        pragmas = re.findall('[^;]\s*#pragma\s+(.+?)\s+(.+?)$', source, re.MULTILINE)
        for pragma in pragmas:
            cmd = pragma[0].lower()
            if cmd == "stage":
                stage = pragma[1].lower()
                if stage == "vertex":
                    shader["stage"] = "Vertex"
                elif stage == "fragment" or stage == "pixel":
                    shader["stage"] = "Fragment"
                else:
                    print "Invalid shader stage: %s for shader %s" % (stage, filename)
            elif cmd == "sampler":
                split = re.split(':', pragma[1])
                if len(split) == 2:
                    name = split[0]
                    try:
                        value = int(split[1])
                    except ValueError:
                        value = 0
                        print "Sampler value must be a integer (%s = %s)" % (name, split[1])

                    try:
                        if uniforms[name] == "sampler2D":
                            sdef.samplers.append({"name": name, "value": value})
                        else:
                            print "Uniform %s is not a sampler. (in #pragma sampler)" % (uniforms[name])
                    except KeyError:
                        print "Sampler %s is not defined in this shader" % (name)
                else:
                    print "Invalid sampler syntax: `%s', expecting exactly one ':'" % (pragma[1])


        g_samplers.add(sdef)

        return index
    else:
        return g_shader_sources[filename]["index"]

def write_output():
    hpp_in_f = open(g_root_dir + "shaderdefs.hpp.in")
    cpp_in_f = open(g_root_dir + "shaderdefs.cpp.in")

    hpp_in = yield_parts("shaderdefs.hpp.in", hpp_in_f.read())
    cpp_in = yield_parts("shaderdefs.cpp.in", cpp_in_f.read())

    hpp_in_f.close()
    cpp_in_f.close()

    hpp = open("gen/shaderdefs.hpp", 'w')
    cpp = open("gen/shaderdefs.cpp", 'w')

    hpp.write(hpp_in[0])
    cpp.write(cpp_in[0])

    cpp.write("namespace shaderdefs {\n");

    hpp.write("enum ShaderId {\n");
    first = True
    for program in g_programs:
        hpp.write("\tShader_%s%s,\n" % (program, (" = 0" if first else "")))
        first = False

    hpp.write("\t_Shader_Count\n};\n")

    hpp.write("enum UniformId {\n")
    cpp.write("const char* uniform_names[_Uniform_Count] = {")
    first = True
    for uniform in g_uniforms:
        hpp.write("\tUniform_%s%s,\n" % (uniform, (" = 0" if first else "")))
        first = False
        cpp.write("\"%s\", " % uniform)

    cpp.write("};\n")
    hpp.write("\t_Uniform_Count\n};\n")

    hpp.write("namespace shaderdefs {\n");

    index = 0
    for udef in g_uniformdefs:
        udef.index = index
        cpp.write("static const int _uniformdefs%d[] = { " % index)
        for uniform in udef.uniforms:
            cpp.write("Uniform_%s, " % (uniform))
        cpp.write(" -1};\n")
        index += 1

    index = 0
    for sdef in g_samplers:
        sdef.index = index
        cpp.write("static const SamplerDef _samplerdefs%d[] = {\n" % index)
        for sampler in sdef.samplers:
            cpp.write("\t{ \"%s\", %d },\n" % (sampler["name"],sampler["value"]))
        cpp.write("\t{ nullptr}\n};\n")
        index += 1

    hpp.write("enum ShaderSourceId {\n")
    cpp.write("ShaderSourceDef stage_sources[_ShaderSource_Count] = {\n")

    first = True
    for name in g_shader_sources:
        shader = g_shader_sources[name]
        if shader["stage"] == None:
            continue
        hpp.write("\tShaderSource_%s%s,\n" % (shader["enum"], (" = 0" if first else "")))
        first = False

        cpp.write("{ \"%s\", u8R\"SHADER_SOURCE(\n" % name);
        cpp.write(shader["source"])
        cpp.write(")SHADER_SOURCE\",\nimpl::ShaderStage_%s, _samplerdefs%d, _uniformdefs%d },\n"
                % (shader["stage"], shader["samplers"].index, shader["uniforms"].index)
                )

    hpp.write("\t_ShaderSource_Count\n};\n")
    cpp.write("};\n")

    cpp.write("ShaderProgramDef programs[_Shader_Count] = {\n");

    for name in g_programs:
        shaders = g_programs[name]
        cpp.write("\t{ \"%s\", { %s } },\n" % (name, ", ".join(str(x) for x in shaders)))

    cpp.write("};\n")



    hpp.write("extern ShaderSourceDef stage_sources[_ShaderSource_Count];\n")
    hpp.write("extern ShaderProgramDef programs[_Shader_Count];\n")
    hpp.write("extern const char* uniform_names[_Uniform_Count];\n")

    hpp.write("}\n")
    cpp.write("}\n")

    hpp.write(hpp_in[1])
    cpp.write(cpp_in[1])

    hpp.close()
    cpp.close()


def yield_parts(filename,in_src):
    pos = in_src.find("<yield>")
    if pos == -1:
        print "%s doesn't contain a <yield>" % (filename)
        exit(-1)

    return (in_src[:pos], in_src[pos+len("<yield>"):])


main()
