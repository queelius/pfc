# MkDocs Documentation Deployment Guide

This guide explains how to deploy the PFC documentation to GitHub Pages.

## Prerequisites

1. Python 3.8 or later
2. pip (Python package manager)
3. Git repository with GitHub remote
4. Push access to the repository

## Installation

Install MkDocs and required plugins:

```bash
# From the project root directory
pip install -r requirements.txt

# Or install individually
pip install mkdocs mkdocs-material pymdown-extensions mkdocs-awesome-pages-plugin
```

## Local Preview

Preview the documentation locally before deploying:

```bash
# Start development server
mkdocs serve

# Open browser to http://127.0.0.1:8000/
```

The server will auto-reload when you make changes to documentation files.

## Building Documentation

Build static HTML files:

```bash
mkdocs build

# Output will be in site/ directory
# You can preview by opening site/index.html in a browser
```

## Deployment to GitHub Pages

### Option 1: Automatic Deployment (Recommended)

Use MkDocs built-in deployment command:

```bash
# Deploy to gh-pages branch
mkdocs gh-deploy

# Or with custom commit message
mkdocs gh-deploy -m "Update documentation [ci skip]"
```

This command will:
1. Build the documentation
2. Push to the gh-pages branch
3. Update GitHub Pages automatically

### Option 2: Manual Deployment

If you prefer manual control:

```bash
# Build the docs
mkdocs build

# Create/switch to gh-pages branch
git checkout gh-pages

# Copy built files
cp -r site/* .

# Commit and push
git add .
git commit -m "Update documentation"
git push origin gh-pages
```

## GitHub Pages Configuration

After first deployment, configure GitHub Pages:

1. Go to your repository on GitHub
2. Navigate to Settings > Pages
3. Under "Source", select:
   - Branch: gh-pages
   - Folder: / (root)
4. Click Save

Your documentation will be available at:
```
https://<username>.github.io/<repository>/
```

For this project:
```
https://spinozarabel.github.io/pfc/
```

## Continuous Integration (Optional)

Automate deployment with GitHub Actions:

Create `.github/workflows/docs.yml`:

```yaml
name: Deploy Documentation

on:
  push:
    branches:
      - main
    paths:
      - 'docs/**'
      - 'mkdocs.yml'

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.x'

      - name: Install dependencies
        run: |
          pip install -r requirements.txt

      - name: Deploy docs
        run: |
          git config user.name github-actions
          git config user.email github-actions@github.com
          mkdocs gh-deploy --force
```

This will automatically deploy documentation when you push changes to the main branch.

## Troubleshooting

### Permission Denied

If you get permission errors:

```bash
# Ensure you're authenticated
git config credential.helper store
git push  # Enter credentials
```

### Build Errors

Check for:
- Missing dependencies: `pip install -r requirements.txt`
- Invalid markdown syntax in docs/
- Broken links in navigation (mkdocs.yml)

### GitHub Pages Not Updating

1. Check the gh-pages branch exists
2. Verify GitHub Pages is enabled in repository settings
3. Wait a few minutes for GitHub to process changes
4. Clear browser cache

## Documentation Structure

```
docs/
├── index.md                 # Home page
├── guides/
│   ├── installation.md
│   ├── quick-start.md
│   ├── design-principles.md
│   ├── custom-codecs.md
│   ├── performance.md
│   ├── error-handling.md
│   └── best-practices.md
├── api/
│   ├── overview.md
│   ├── core.md
│   ├── codecs.md
│   ├── numeric-codecs.md
│   ├── packed.md
│   ├── algebraic.md
│   ├── stl-integration.md
│   ├── algorithms.md
│   ├── coordinates.md
│   ├── error-handling.md
│   ├── compression.md
│   └── crc.md
└── examples/
    ├── basic.md
    ├── codecs.md
    ├── containers.md
    ├── algebraic.md
    ├── compression.md
    └── advanced.md
```

## Updating Documentation

Workflow for documentation updates:

1. Edit markdown files in docs/
2. Preview locally: `mkdocs serve`
3. Build and test: `mkdocs build`
4. Commit changes: `git add docs/ mkdocs.yml && git commit -m "Update docs"`
5. Deploy: `mkdocs gh-deploy`

## Custom Domain (Optional)

To use a custom domain:

1. Add CNAME file to docs/:
   ```bash
   echo "docs.yourproject.com" > docs/CNAME
   ```

2. Configure DNS with your provider:
   - Add CNAME record pointing to `<username>.github.io`

3. Update mkdocs.yml:
   ```yaml
   site_url: https://docs.yourproject.com/
   ```

4. Redeploy: `mkdocs gh-deploy`

## Versioning (Advanced)

For versioned documentation, use mike:

```bash
pip install mike

# Deploy version
mike deploy --push --update-aliases 1.0 latest

# Set default version
mike set-default --push latest

# List versions
mike list
```

## Support

For issues:
- MkDocs documentation: https://www.mkdocs.org/
- Material theme: https://squidfunk.github.io/mkdocs-material/
- GitHub Pages: https://docs.github.com/en/pages
