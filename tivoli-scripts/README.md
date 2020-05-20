# Tivoli scripts

Written using TypeScript and Angular.

## Development

-   You'll want to use `yarn` for development because `npm` is ridiculously slow with dependencies, espcially for Angular which will be even slower if you don't have a high speed NVMe drive. It's so frustrating but Node is planning to fix their dependency system.

    ```bash
    npm i -g yarn

    yarn deps
    # or
    cd scripts
    yarn install
    cd ../frontend
    yarn install
    ```

-   Open two terminals. In VSCode you can side-by-side them in <kbd>CTRL</kbd>+<kbd>`</kbd>

    -   ```bash
        cd scripts
        yarn start
        ```
    -   ```bash
        cd frontend
        yarn start
        ```

    Which will run both scripts and frontend in watch mode.

-   You'll then find `tivoli-scripts/dist/tivoli.js` which is a script you can load in interface.
