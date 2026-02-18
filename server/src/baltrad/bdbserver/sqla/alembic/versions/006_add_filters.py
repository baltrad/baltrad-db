from alembic import op
import sqlalchemy as sa

revision = '006'
down_revision = '005'


def upgrade():
    op.create_table(
        'bdb_filters',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('name', sa.Text(), nullable=False),
        sa.Column('expression', sa.Text(), nullable=False),
        sa.PrimaryKeyConstraint('id')
    )


def downgrade():
    op.drop_table('bdb_filters')
