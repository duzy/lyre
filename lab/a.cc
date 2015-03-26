class IFoo
{
public:
    virtual ~IFoo() {}
    virtual int foo() = 0;
    virtual void bar() = 0;
    void a();
};

void IFoo::a() { this->foo(); }

class Foo : public IFoo
{
    int value;

public:
    explicit Foo(int v) : value(v) {}
    virtual int foo();
    virtual void bar();
    void b();
};

void Foo::b() {}
int Foo::foo() { return value; }
void Foo::bar()
{
    this->a();
    this->b();

    IFoo * i = reinterpret_cast<IFoo*>(this);
    i->a();
    i->foo();
}

class Foobar : public IFoo
{
    int value;
    int factor;

public:
    explicit Foobar(int v, int f) : value(v), factor(f) {}
    virtual int foo() { return value * factor; }
};

static void cal(int n, ...)
{
}

static void str(const char * s, ...)
{
}

static void test_foo(Foo foo)
{
    IFoo *p = &foo;

    foo.foo();

    p->foo();
}

int main(int argc, char**argv)
{
    int num = 0;
    cal(num, num);

    const char *s = "foo";
    str(s, s, num);
    str("%s", s, num);

    IFoo *ptr = new Foo(100);

    int v = ptr->foo();

    ptr->a();
    reinterpret_cast<Foo*>(ptr)->b();

    test_foo(*reinterpret_cast<Foo*>(ptr));

    delete ptr;
    return 0;
}
