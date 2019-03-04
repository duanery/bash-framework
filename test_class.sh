#!/bin/bash
. $(dirname $0)/include.h
include class.h

#============================================================
Class TestEmbed {
    TestEmbed::TestEmbed() {
        echo TestEmbed ok
    }
    TestEmbed::~TestEmbed() {
        echo ~TestEmbed ok
    }
}


Class Animal {
    public string limb
    public string eye
    public string nose
    Animal::Animal() {
        Animal::eye = "black"
        Animal::nose = "red"
        Animal::limb = "yello"
    }
    Animal::~Animal() {
        echo bye~~
    }
    Animal::tostring() {
        echo eye $(Animal::eye) nose $(Animal::nose) limb $(Animal::limb)
    }
    Animal::sleep() {
        sleep $1
    }
}
FiveSenses=(
    string eyes
    string lips
    string ears
    string tongue
    string nose
)
Class Stupid {
    private string _desc
    private uint32_t _value
    Stupid::Stupid() { :; }
    Stupid::~Stupid() { :; }
    Stupid::desc() {
        [ $# -eq 0 ] && {
            echo $(Stupid::_desc)
            return
        }
        Stupid::_desc = "$@"
    }
    Stupid::value() {
        [ $# -eq 0 ] && {
            echo $(Stupid::_value)
            return
        }
        Stupid::_value = $1
    }
}
Class Human { : public Animal
    public string name
    public uint32_t height
    private string six
    private struct FiveSenses five
    public class Stupid stupid
    
    private eat
    Human::eat() {
        echo $@
    } 
    Human::Human() {
        Human::five.ears = "big"
        Human::limb = "--"
        Human::name = "human"
        Human::height = "160"
        Human::six = "six"
        $(Human::stupid).desc 'It is foolish to waste time.'
        $(Human::stupid).value 67
    }
    Human::~Human() {
        :
    }
    Human::tostring() {
        class TestEmbed em
        echo human $(Human::name) height $(Human::height) $(Animal::tostring) \
            eyes $(Human::five.eyes) lips $(Human::five.lips) ears $(Human::five.ears) \
            tongue $(Human::five.tongue) nose $(Human::five.nose)
        delete em
    }
    @ DECORATOR Human::tostring
    Human::speak() {
        echo hello wrold $@
    }
    Human::putout() {
        putout
    }
}
Class Men { : public Human
    private string sex
    Men::Men() {
        Men::sex = "men"
        #Men::five.eyes = "small"
    }
    Men::~Men() {
        echo "I am a men, bye~~"
    }
}

Human:++ () {
    Human::height ++
}
Human:+= () {
    if $1 instanceof Human ; then
        Human::height += $($1.height)
    fi
}

DECORATOR() {
    myDECORATOR() {
        echo =========================
        $@
        echo =========================
    }
    __return=myDECORATOR
}

class Men mark

#declare -p mark Animal Human Men Stupid
#declare -f $(compgen -A 'function' "Stupid::")
#declare -f $(compgen -A 'function' "Animal::")
#declare -f $(compgen -A 'function' "Human::") 
#declare -f $(compgen -A 'function' "Men::")  
#declare -f $(compgen -A 'function' "mark")
#declare -f mark Animal Human Men Stupid
#exit



mark.eat dd
mark.speak ddd
#declare -p Human
#declare -f Human mark.eat mark.speak Human::name Human::height Human::six
#declare -F
mark.eye += '-g'
mark.name = "mark"
#mark.five.nose = 'bigger'
declare -p $(mark.stupid)
declare -f $(compgen -A 'function' "$(mark.stupid)")

mark.tostring
typeof mark
mark.height += 2
if mark instanceof Human; then
    echo =ok=
fi
Object* newobj = new Human
mark += $newobj
if $newobj instanceof Human; then
    delete* newobj
    echo =yes=
fi
mark ++
mark.height

$(typeof mark) kube
kube.speak "I am kube"
delete kube
exit
echo ======
compgen -A 'function' "__struct_"
compgen -A 'function' "Animal::"
compgen -A 'function' "Human::"
compgen -A 'function' "Men::"
compgen -A 'function' "mark"

declare -p mark Animal Human Men
#declare -f $(compgen -A 'function' "Animal::")
declare -f $(compgen -A 'function' "Human::") 
declare -f $(compgen -A 'function' "Men::")  
declare -f $(compgen -A 'function' "mark")
declare -f mark Animal Human Men
delete mark

exit


class Human;
Object* newobj = new Human
#$newobj.tostring
delete* newobj
exit
for((i=0;i<1000;i++))
do
^_^ newobj = new Human
#$newobj.tostring
delete* newobj
done

exit