module Router = {
  type route =
    | Index
    | About(string)
    | NotFound;

  let urlPathToStr = UrlPath.toUrl;

  let pathToRoute = urlPath =>
    switch (urlPath) {
    | [] => Index
    | ["about", sub] => About(sub)
    | _ => NotFound
    };

  let routeToPath = route =>
    switch (route) {
    | Index => []
    | About(sub) => ["about", sub]
    | NotFound => ["404"]
    };

  let href = (~to_: route): string => routeToPath(to_) |> urlPathToStr;
};

module Pages = {
  let about =
    <div>
      <div> "About"->ReasonReact.string </div>
      <a href=Router.(href(~to_=Index))> "Home"->ReasonReact.string </a>
    </div>;

  let index =
    <div>
      <div> "Home"->ReasonReact.string </div>
      <a href=Router.(href(~to_=About("1")))>
        "About"->ReasonReact.string
      </a>
    </div>;
};

let applyLayout = children =>
  <html>
    <head> <link rel="stylesheet" href="my.css" /> </head>
    <main> children </main>
  </html>;

module Transform = {
  let template = (~index as _, compileUnit) => {
    let (path, component) = compileUnit;

    let applied =
      switch (path) {
      | ["about", "1"] => applyLayout(component)
      | ["about", _] => applyLayout(<div id="1"> component </div>)
      | _ => applyLayout(component)
      };

    (path, applied);
  };
};

module ReactCompiler = {
  module Component = {
    type t = ReasonReact.reactElement;
  };

  include Compiler.Make(Component, Filesystem.Node);

  module Transform = {
    let template = (~index as _, compileUnit: CompileUnit.t) => "";
  };

  let processComponent = (comp: Component.t) =>
    ReactDOMServerRe.renderToStaticMarkup(comp);
};

let config: list(ReactCompiler.CompileUnit.t) = [
  Bundle({id: "navigation", entry: "./site/navigation.bs.js"}),
  Page({urlPath: [], component: Pages.index}),
  Page({urlPath: ["about"], component: Pages.about}),
];

let compiler = ReactCompiler.make();

config |> ReactCompiler.compileAll(compiler);
